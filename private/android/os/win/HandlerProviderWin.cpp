/*
 * Copyright (C) 2016 Naver Corp. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "HandlerProviderWin.h"

#include "MessageCopyData.h"
#include <android/os/Messenger.h>
#include <java/lang/System.h>

#include <array>
#include <mutex>

#include <assert>
#include <mmsystem.h>

// Derived from Code from Webkit (https://webkit.org/) under LGPL v2 and BSD licenses (https://webkit.org/licensing-webkit/)

// These aren't in winuser.h with the MSVS 2003 Platform SDK, 
// so use default values in that case.
#ifndef USER_TIMER_MINIMUM
#define USER_TIMER_MINIMUM 0x0000000A
#endif

#ifndef USER_TIMER_MAXIMUM
#define USER_TIMER_MAXIMUM 0x7FFFFFFF
#endif

#ifndef QS_RAWINPUT
#define QS_RAWINPUT         0x0400
#endif

namespace android {
namespace os {

static const LPWSTR kMessageWindowClassName = L"MessageWindow";

const int timerResolution = 1; // To improve timer resolution, we call timeBeginPeriod/timeEndPeriod with this value to increase timer resolution to 1ms.
const int highResolutionThresholdMsec = 16; // Only activate high-res timer for sub-16ms timers (Windows can fire timers at 16ms intervals without changing the system resolution).
const int stopHighResTimerInMsec = 300; // Stop high-res timer after 0.3 seconds to lessen power consumption (we don't use a smaller time since oscillating between high and low resolution breaks timer accuracy on XP).

static UINT timerFiredMessage = 0;

enum {
    sharedTimerID = 1000,
    endHighResTimerID = 1001,
};

std::unique_ptr<HandlerProvider> HandlerProvider::create(Handler& client)
{
    return std::unique_ptr<HandlerProvider>(new HandlerProviderWin(client));
}

class HandlerProviderWindow : public HandlerProvider {
    friend class HandlerProviderWin;
public:
    ~HandlerProviderWindow();

    bool start() override;
    bool startAtTime(std::chrono::milliseconds) override;
    void stop() override;

    void close();

private:
    HandlerProviderWindow(Handler&);

    static void registerMessageWindowClass();

    static LRESULT CALLBACK messageWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    static LRESULT messageWindowProcInternal(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    static void NTAPI messageWindowQueueTimerCallback(PVOID parameter, BOOLEAN);

    HWND m_messageWindow;

    UINT m_activeTimerID;
    bool m_shouldUseHighResolutionTimers;
    HANDLE m_timerQueue;
    HANDLE m_timerQueueTimer;
    bool m_highResTimerActive;
    bool m_processingCustomTimerMessage;
    LONG m_pendingTimers;
};

HandlerProviderWin::HandlerProviderWin(Handler& client)
    : HandlerProvider(client)
    , m_window(*new HandlerProviderWindow(client))
{
}

HandlerProviderWin::~HandlerProviderWin()
{
    m_window.close();
}

bool HandlerProviderWin::start()
{
    return m_window.start();
}

bool HandlerProviderWin::startAtTime(std::chrono::milliseconds uptimeMillis)
{
    return m_window.startAtTime(uptimeMillis);
}

void HandlerProviderWin::stop()
{
    m_window.stop();
}

HWND HandlerProviderWin::messageWindowHandle(HandlerProvider& handler)
{
    return static_cast<HandlerProviderWin&>(handler).m_window.m_messageWindow;
}

HandlerProviderWindow::HandlerProviderWindow(Handler& client)
    : HandlerProvider(client)
    , m_activeTimerID(0)
    , m_shouldUseHighResolutionTimers(true)
    , m_timerQueue(::CreateTimerQueue())
    , m_timerQueueTimer(0)
    , m_highResTimerActive(0)
    , m_processingCustomTimerMessage(false)
    , m_pendingTimers(0)
{
    registerMessageWindowClass();

    m_messageWindow = ::CreateWindow(kMessageWindowClassName, 0, 0,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, HWND_MESSAGE, 0, 0, this);
}

HandlerProviderWindow::~HandlerProviderWindow()
{
}

void HandlerProviderWindow::registerMessageWindowClass()
{
    static std::once_flag onceFlag;
    std::call_once(onceFlag, [=]{
        WNDCLASS windowClass = { 0 };
        windowClass.lpfnWndProc = HandlerProviderWindow::messageWindowProc;
        windowClass.cbWndExtra = sizeof(HandlerProviderWindow*) * 2;
        windowClass.lpszClassName = kMessageWindowClassName;

        timerFiredMessage = ::RegisterWindowMessage(L"HandlerTimerFired");

        if (!::RegisterClass(&windowClass))
            assert(0);
    });
}

#define GWLP_HANDLERPTR(n) ((n) * sizeof(HandlerProviderWindow*))

LRESULT CALLBACK HandlerProviderWindow::messageWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (HandlerProviderWindow* handler = static_cast<HandlerProviderWindow*>(reinterpret_cast<void*>(::GetWindowLongPtr(hWnd, GWLP_HANDLERPTR(0)))))
        return messageWindowProcInternal(hWnd, message, wParam, lParam);

    if (message == WM_CREATE) {
        LPCREATESTRUCT createStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);

        reinterpret_cast<void*>(::SetWindowLongPtr(hWnd, GWLP_HANDLERPTR(0),
                reinterpret_cast<LONG_PTR>(createStruct->lpCreateParams)));
        return 0;
    }

    if (message == WM_CLOSE) {
        HandlerProviderWindow* handler = static_cast<HandlerProviderWindow*>(reinterpret_cast<void*>(::GetWindowLongPtr(hWnd, GWLP_HANDLERPTR(1))));
        ::SetWindowLongPtr(hWnd, GWLP_HANDLERPTR(1), 0);
        delete handler;
    }

    return ::DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT HandlerProviderWindow::messageWindowProcInternal(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HandlerProviderWindow* handler = static_cast<HandlerProviderWindow*>(reinterpret_cast<void*>(::GetWindowLongPtr(hWnd, GWLP_HANDLERPTR(0))));
    switch (message) {
    case WM_COPYDATA: {
        HWND replyTo = (HWND)wParam;
        COPYDATASTRUCT* data = (COPYDATASTRUCT*)lParam;
        DWORD processIdentifier = data->dwData;
        MessageCopyData::Data values;
        memcpy(&values, data->lpData, data->cbData);
        Message message;
        MessageCopyData::unpack(message, values);
        message.replyTo = new Messenger((IBinder)replyTo);
        handler->receivedMessage(message);
        return 0;
    }
    case WM_TIMER:
        if (wParam == sharedTimerID) {
            ::KillTimer(hWnd, sharedTimerID);
            handler->performMessages();
        } else if (wParam == endHighResTimerID) {
            ::KillTimer(hWnd, endHighResTimerID);
            handler->m_highResTimerActive = false;
            ::timeEndPeriod(timerResolution);
        }
        return 0;
    default:
        if (message == timerFiredMessage) {
            ::InterlockedExchange(&handler->m_pendingTimers, 0);
            handler->m_processingCustomTimerMessage = true;
            handler->performMessages();
            handler->m_processingCustomTimerMessage = false;
            return 0;
        }
        break;
    }

    return ::DefWindowProc(hWnd, message, wParam, lParam);
}

void NTAPI HandlerProviderWindow::messageWindowQueueTimerCallback(PVOID parameter, BOOLEAN)
{
    HandlerProviderWindow* handler = static_cast<HandlerProviderWindow*>(parameter);
    handler->start();
}

bool HandlerProviderWindow::start()
{
    if (::InterlockedIncrement(&m_pendingTimers) == 1)
        ::PostMessage(m_messageWindow, timerFiredMessage, 0, 0);
    return true;
}

bool HandlerProviderWindow::startAtTime(std::chrono::milliseconds uptimeMillis)
{
    std::chrono::milliseconds delayMillis = uptimeMillis - System::currentTimeMillis();

    DWORD dueTime;
    if (delayMillis.count() > USER_TIMER_MAXIMUM)
        dueTime = USER_TIMER_MAXIMUM;
    else if (delayMillis.count() < 0)
        dueTime = 0;
    else
        dueTime = static_cast<DWORD>(delayMillis.count());

    if (dueTime == 0) {
        start();
        return true;
    }

    bool timerSet = false;

    if (m_shouldUseHighResolutionTimers) {
        if (dueTime < highResolutionThresholdMsec) {
            if (!m_highResTimerActive) {
                m_highResTimerActive = true;
                ::timeBeginPeriod(timerResolution);
            }
            ::SetTimer(m_messageWindow, endHighResTimerID, stopHighResTimerInMsec, 0);
        }

        DWORD queueStatus = LOWORD(::GetQueueStatus(QS_PAINT | QS_MOUSEBUTTON | QS_KEY | QS_RAWINPUT));

        // Win32 has a tri-level queue with application messages > user input > WM_PAINT/WM_TIMER.

        // If the queue doesn't contains input events, we use a higher priorty timer event posting mechanism.
        if (!(queueStatus & (QS_MOUSEBUTTON | QS_KEY | QS_RAWINPUT))) {
            if (dueTime < USER_TIMER_MINIMUM && !m_processingCustomTimerMessage && !(queueStatus & QS_PAINT)) {
                // Call PostMessage immediately if the timer is already expired, unless a paint is pending.
                // (we prioritize paints over timers)
                if (::InterlockedIncrement(&m_pendingTimers) == 1)
                    ::PostMessage(m_messageWindow, timerFiredMessage, 0, 0);
                timerSet = true;
            } else {
                // Otherwise, delay the PostMessage via a CreateTimerQueueTimer
                if (m_timerQueueTimer)
                    ::DeleteTimerQueueTimer(m_timerQueue, m_timerQueueTimer, 0);
                timerSet = ::CreateTimerQueueTimer(&m_timerQueueTimer, m_timerQueue, messageWindowQueueTimerCallback, this, dueTime, 0, WT_EXECUTEINTIMERTHREAD | WT_EXECUTEONLYONCE);
            }
        }
    }

    if (timerSet) {
        if (m_activeTimerID) {
            ::KillTimer(m_messageWindow, m_activeTimerID);
            m_activeTimerID = 0;
        }
    } else {
        m_activeTimerID = ::SetTimer(m_messageWindow, sharedTimerID, dueTime, 0);
        m_timerQueueTimer = 0;
    }

    return true;
}

void HandlerProviderWindow::stop()
{
    if (m_timerQueue && m_timerQueueTimer) {
        ::DeleteTimerQueueTimer(m_timerQueue, m_timerQueueTimer, 0);
        m_timerQueueTimer = 0;
    }

    if (m_activeTimerID) {
        ::KillTimer(m_messageWindow, m_activeTimerID);
        m_activeTimerID = 0;
    }
}

void HandlerProviderWindow::close()
{
    ::SetWindowLongPtr(m_messageWindow, GWLP_HANDLERPTR(0), 0);
    ::SetWindowLongPtr(m_messageWindow, GWLP_HANDLERPTR(1), reinterpret_cast<LONG_PTR>(this));
    ::PostMessageA(m_messageWindow, WM_CLOSE, 0, 0);
}

} // namespace os
} // namespace android
