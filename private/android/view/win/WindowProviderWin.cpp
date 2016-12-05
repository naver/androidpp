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

#include "WindowProviderWin.h"

#include <android/os/Looper.h>
#include <android/view/InputDevice.h>
#include <android/view/KeyEvent.h>
#include <android/view/KeyEventPrivate.h>
#include <android/view/MotionEvent.h>
#include <android/view/MotionEventPrivate.h>
#include <android/view/ViewHostWindow.h>
#include <android/view/appkit/CompositionClause.h>
#include <android/view/appkit/win/CursorWin.h>
#include <android/view/inputmethod/InputConnectionPrivate.h>

#include <platforms/LogHelper.h>

#include <assert>
#include <limits>

namespace android {
namespace view {

std::unique_ptr<WindowProvider> WindowProvider::create(WindowHandle parentWindow, const Rect& clientRect,
    ViewHostWindow& viewHost)
{
    return std::unique_ptr<WindowProvider>(new WindowProviderWin(parentWindow, clientRect, viewHost));
}

std::map<WindowProviderWin*, std::shared_ptr<ViewHostWindow>> WindowProviderWin::m_popupWindowHolder;

WindowProviderWin::WindowProviderWin(WindowHandle parentWindow, const Rect& clientRect, ViewHostWindow& viewHost)
    : WindowProvider(viewHost)
    , m_cursor(Cursor::systemCursor(Cursor::Type::Default))
    , m_pendingCursor(Cursor::systemCursor(Cursor::Type::Default))
    , m_mouseEntered(false)
    , m_imm32(NULL)
    , m_batchCount(0)
    , m_isCompositing(false)
    , m_requestedCursorUpdates(false)
{
    Create(reinterpret_cast<HWND>(parentWindow), ATL::_U_RECT(static_cast<RECT>(clientRect)), _T("WindowProviderHWND"),
        WS_CHILDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

    loadIMM();
    enableInputMethod(false);
}

WindowProviderWin::~WindowProviderWin()
{
    DestroyWindow();
}

bool WindowProviderWin::platformCreatePopupWindow(const Rect& clientRect)
{
    Create(createPopupWindow(clientRect), ATL::_U_RECT(static_cast<RECT>(clientRect)), _T("PopupWindowHWND"),
        WS_CHILDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

    if (!hwnd())
        return false;

    m_popupWindowHolder[this] = std::shared_ptr<ViewHostWindow>(&m_viewHost);
    return true;
}

void WindowProviderWin::platformClosePopupWindow()
{
    if (m_popupWindowHolder[this])
        ::DestroyWindow(::GetParent(hwnd()));
}

WindowHandle WindowProviderWin::platformWindowHandle() const
{
    return hwnd();
}

void WindowProviderWin::platformStartInputMethod(bool enable)
{
    enableInputMethod(enable);
}

void WindowProviderWin::platformResetInputMethod()
{
    resetIME();
}

void WindowProviderWin::platformSetCursor(const std::shared_ptr<Cursor>& cursor)
{
    if (m_cursor->getType() == cursor->getType() && cursor->getType() != Cursor::Type::Custom)
        return;

    if (!m_mouseEntered)
        return;

    m_pendingCursor = cursor;

    // Trick to force a cursor update.
    POINT cursorPoint;
    ::GetCursorPos(&cursorPoint);
    ::SetCursorPos(cursorPoint.x, cursorPoint.y);
}

void WindowProviderWin::platformSetWindowFocus(bool focus)
{
    if (!focus)
        return;

    SetFocus();
}

void WindowProviderWin::platformSetWindowCaptureMouse(bool capture)
{
    (capture) ? (void)SetCapture() : (void)ReleaseCapture();
}

void WindowProviderWin::platformSetWindowPosition(int32_t x, int32_t y, int32_t width, int32_t height)
{
    SetWindowPos(NULL, x, y, width, height, SWP_NOZORDER | SWP_NOACTIVATE);
}

void WindowProviderWin::platformSendWindowMove()
{
    ::PostMessage(hwnd(), WM_MOVE, 0, 0);
}

float WindowProviderWin::platformDeviceScaleFactor() const
{
    CClientDC dc(hwnd());
    return ::GetDeviceCaps(dc, LOGPIXELSX) / 96.0f;
}

HWND WindowProviderWin::hwnd() const
{
    return static_cast<HWND>(*this);
}

HWND WindowProviderWin::createPopupWindow(const Rect& clientRect)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style          = CS_DBLCLKS;
    wcex.lpfnWndProc    = WindowProviderWin::popupWindowWndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = sizeof(WindowProviderWin*);
    wcex.hInstance      = ::GetModuleHandle(0);
    wcex.hIcon          = 0;
    wcex.hCursor        = ::LoadCursor(0, IDC_ARROW);
    wcex.hbrBackground  = 0;
    wcex.lpszMenuName   = 0;
    wcex.lpszClassName  = L"PopupWindowHWND";
    wcex.hIconSm        = 0;

    ::RegisterClassEx(&wcex);

    HWND windowHandle = CreateWindowEx(0, L"PopupWindowHWND", 0, WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT, clientRect.right, clientRect.bottom, 0, 0, 0, this);

    return windowHandle;
}

LRESULT CALLBACK WindowProviderWin::popupWindowWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LONG_PTR longPtr = ::GetWindowLongPtr(hWnd, 0);

    if (message == WM_CREATE) {
        LPCREATESTRUCT createStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
        // Associate the WindowProviderWin with the window.
        ::SetWindowLongPtr(hWnd, 0, (LONG_PTR)createStruct->lpCreateParams);
    } else if (message == WM_DESTROY) {
        if (WindowProviderWin* windowProvider = reinterpret_cast<WindowProviderWin*>(longPtr))
            m_popupWindowHolder.erase(windowProvider);
    } else {
        if (WindowProviderWin* windowProvider = reinterpret_cast<WindowProviderWin*>(longPtr))
            return windowProvider->popupWindowEventHandler(hWnd, message, wParam, lParam);
    }

    return ::DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT WindowProviderWin::popupWindowEventHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_SIZE:
        RECT clientRect;
        ::GetClientRect(hWnd, &clientRect);
        m_viewHost.setWindowPosition(clientRect.left, clientRect.top, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);
        m_viewHost.windowSizeChanged(LOWORD(lParam), HIWORD(lParam), (ViewHostWindow::Resize)wParam);
        break;
    default:
        break;
    }

    return ::DefWindowProc(hWnd, message, wParam, lParam);;
}

static PointF pointF(const CPoint& point)
{
    return PointF(static_cast<float>(point.x), static_cast<float>(point.y));
}

PointF WindowProviderWin::toScreen(const CPoint& point)
{
    CPoint pt(point);
    ClientToScreen(&pt);
    return pointF(pt);
}

PointF WindowProviderWin::fromScreen(const CPoint& point)
{
    CPoint pt(point);
    ScreenToClient(&pt);
    return pointF(pt);
}

int WindowProviderWin::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    SetMsgHandled(FALSE);

    ShowWindow(TRUE);
    UpdateWindow();

    m_viewHost.windowCreated();
    return 0;
}

void WindowProviderWin::OnDestroy()
{
    SetMsgHandled(FALSE);

    m_viewHost.windowDestroyed();
}

void WindowProviderWin::OnSetFocus(CWindow wndOld)
{
    SetMsgHandled(FALSE);

    m_viewHost.windowFocused(true);
}

void WindowProviderWin::OnKillFocus(CWindow wndFocus)
{
    SetMsgHandled(FALSE);

    m_viewHost.windowFocused(false);

    resetIME();
}

BOOL WindowProviderWin::OnSetCursor(CWindow wnd, UINT nHitTest, UINT message)
{
    auto& cursorToShow = m_pendingCursor;

    if (nHitTest != HTCLIENT)
        cursorToShow = Cursor::systemCursor(Cursor::Type::Pointer);

    if (cursorToShow->getType() == Cursor::Type::Retain)
        return TRUE;
    else if (cursorToShow->getType() == Cursor::Type::Default)
        return FALSE;

    ::SetCursor(std::static_pointer_cast<appkit::CursorWin>(cursorToShow)->nativeCursor());
    m_cursor = m_pendingCursor;
    m_pendingCursor = Cursor::systemCursor(Cursor::Type::Retain);

    return TRUE;
}

void WindowProviderWin::OnShowWindow(BOOL bShow, UINT nStatus)
{
    SetMsgHandled(FALSE);

    m_viewHost.windowIsVisible((bShow) ? true : false);
}

void WindowProviderWin::OnPaint(CDCHandle dc)
{
    CPaintDC cdc(*this);
    if (cdc.m_ps.fErase) {
        Rect rect(cdc.m_ps.rcPaint);
        m_viewHost.invalidate(rect);
    }
}

BOOL WindowProviderWin::OnEraseBkgnd(CDCHandle dc)
{
    return FALSE;
}

void WindowProviderWin::OnMove(CPoint position)
{
    RECT rect;
    ::GetWindowRect(hwnd(), &rect);
    m_viewHost.windowPositionChanged(rect.left, rect.top);
}

void WindowProviderWin::OnSize(UINT nType, CSize size)
{
    SetMsgHandled(FALSE);

    ViewHostWindow::Resize resize = ViewHostWindow::Restored;
    switch (nType) {
    case SIZE_MINIMIZED:
        resize = ViewHostWindow::Minimized;
        break;
    case SIZE_MAXIMIZED:
        resize = ViewHostWindow::Maximized;
        break;
    case SIZE_RESTORED:
    default:
        break;
    };

    m_viewHost.windowSizeChanged(size.cx, size.cy, resize);
}

static int32_t metaStateForMouseEvent(UINT nFlags)
{
    int32_t modifiers;
    if (nFlags & MK_CONTROL)
        modifiers |= KeyEvent::META_CTRL_ON;
    if (nFlags & MK_SHIFT)
        modifiers |= KeyEvent::META_SHIFT_ON;
    if (HIBYTE(GetKeyState(VK_MENU)) & 0x80)
        modifiers |= KeyEvent::META_ALT_ON;

    return modifiers;
}

static int32_t buttonStateForMouseEvent(UINT nFlags)
{
    int32_t modifiers;
    if (nFlags & MK_LBUTTON)
        modifiers |= MotionEvent::BUTTON_PRIMARY;
    if (nFlags & MK_RBUTTON)
        modifiers |= MotionEvent::BUTTON_SECONDARY;
    if (nFlags & MK_MBUTTON)
        modifiers |= MotionEvent::BUTTON_TERTIARY;

    return modifiers;
}

class MouseEventPrivate final : public MotionEventPrivate {
    friend class MotionEvent;
public:
    MouseEventPrivate(MotionEvent& event, int32_t actionButton, int32_t buttonState, int32_t repeatCount, float wheelDelta);
    ~MouseEventPrivate() = default;

    float wheelDelta;

    float getAxisValue(int32_t axis, int32_t pointerIndex) override;
    float getOrientation(int32_t pointerIndex) override;
};

MouseEventPrivate::MouseEventPrivate(MotionEvent& event, int32_t actionButton, int32_t buttonState, int32_t repeatCount, float wheelDelta)
    : MotionEventPrivate(event)
    , wheelDelta(wheelDelta)
{
    this->actionButton = actionButton;
    this->buttonState = buttonState;
    this->repeatCount = repeatCount;
}

float MouseEventPrivate::getAxisValue(int32_t axis, int32_t pointerIndex)
{
    if (axis == MotionEvent::AXIS_VSCROLL)
        return wheelDelta;
    return std::numeric_limits<float>::quiet_NaN();
}

float MouseEventPrivate::getOrientation(int32_t pointerIndex)
{
    return std::numeric_limits<float>::quiet_NaN();
}

static MotionEvent mouseEvent(std::chrono::milliseconds downTime, int32_t action, const PointF& xy, const PointF& rawXY, UINT nFlags, int32_t actionButton, int32_t repeatCount, float wheelDelta = 0.0f)
{
    MotionEvent event(MotionEvent::obtain(downTime, System::currentTimeMillis(), action, xy.x, xy.y, metaStateForMouseEvent(nFlags)));
    MotionEventPrivate::setPrivate(event, std::make_unique<MouseEventPrivate>(event, actionButton, buttonStateForMouseEvent(nFlags), repeatCount, wheelDelta));
    return event;
}

void WindowProviderWin::OnLButtonDown(UINT nFlags, CPoint point)
{
    m_viewHost.dispatchMouseEvent(mouseEvent(generateDownTime(VK_LBUTTON), MotionEvent::ACTION_BUTTON_PRESS, pointF(point), toScreen(point), nFlags, MotionEvent::BUTTON_PRIMARY, 1));
}

void WindowProviderWin::OnLButtonDoubleClick(UINT nFlags, CPoint point)
{
    m_viewHost.dispatchMouseEvent(mouseEvent(getDownTime(VK_LBUTTON), MotionEvent::ACTION_BUTTON_PRESS, pointF(point), toScreen(point), nFlags, MotionEvent::BUTTON_PRIMARY, 2));
}

void WindowProviderWin::OnLButtonUp(UINT nFlags, CPoint point)
{
    m_viewHost.dispatchMouseEvent(mouseEvent(getDownTime(VK_LBUTTON), MotionEvent::ACTION_BUTTON_RELEASE, pointF(point), toScreen(point), nFlags, MotionEvent::BUTTON_PRIMARY, 1));
}

void WindowProviderWin::OnMButtonDown(UINT nFlags, CPoint point)
{
    m_viewHost.dispatchMouseEvent(mouseEvent(generateDownTime(VK_MBUTTON), MotionEvent::ACTION_BUTTON_PRESS, pointF(point), toScreen(point), nFlags, MotionEvent::BUTTON_TERTIARY, 1));
}

void WindowProviderWin::OnMButtonDoubleClick(UINT nFlags, CPoint point)
{
    m_viewHost.dispatchMouseEvent(mouseEvent(getDownTime(VK_MBUTTON), MotionEvent::ACTION_BUTTON_PRESS, pointF(point), toScreen(point), nFlags, MotionEvent::BUTTON_TERTIARY, 2));
}

void WindowProviderWin::OnMButtonUp(UINT nFlags, CPoint point)
{
    m_viewHost.dispatchMouseEvent(mouseEvent(getDownTime(VK_MBUTTON), MotionEvent::ACTION_BUTTON_RELEASE, pointF(point), toScreen(point), nFlags, MotionEvent::BUTTON_TERTIARY, 1));
}

void WindowProviderWin::OnRButtonDown(UINT nFlags, CPoint point)
{
    m_viewHost.dispatchMouseEvent(mouseEvent(generateDownTime(VK_RBUTTON), MotionEvent::ACTION_BUTTON_PRESS, pointF(point), toScreen(point), nFlags, MotionEvent::BUTTON_SECONDARY, 1));
}

void WindowProviderWin::OnRButtonDoubleClick(UINT nFlags, CPoint point)
{
    m_viewHost.dispatchMouseEvent(mouseEvent(getDownTime(VK_RBUTTON), MotionEvent::ACTION_BUTTON_PRESS, pointF(point), toScreen(point), nFlags, MotionEvent::BUTTON_SECONDARY, 2));
}

void WindowProviderWin::OnRButtonUp(UINT nFlags, CPoint point)
{
    m_viewHost.dispatchMouseEvent(mouseEvent(getDownTime(VK_RBUTTON), MotionEvent::ACTION_BUTTON_RELEASE, pointF(point), toScreen(point), nFlags, MotionEvent::BUTTON_SECONDARY, 1));
}

void WindowProviderWin::OnMouseMove(UINT nFlags, CPoint point)
{
    sendGeneratedMouseEnterEventIfNeeded(nFlags, point);
    m_viewHost.dispatchMouseEvent(mouseEvent(0ms, MotionEvent::ACTION_HOVER_MOVE, pointF(point), toScreen(point), nFlags, 0, 0));
}

void WindowProviderWin::OnMouseLeave()
{
    m_cursor = Cursor::systemCursor(Cursor::Type::Default);
    m_mouseEntered = false;
    POINT point;
    ::GetCursorPos(&point);
    m_viewHost.dispatchMouseEvent(mouseEvent(0ms, MotionEvent::ACTION_HOVER_EXIT, fromScreen(point), pointF(point), 0, 0, 0));
}

BOOL WindowProviderWin::OnMouseWheel(UINT nFlags, short zDelta, CPoint point)
{
    m_viewHost.dispatchMouseEvent(mouseEvent(0ms, MotionEvent::ACTION_SCROLL, fromScreen(point), pointF(point), nFlags, 0, 0, static_cast<float>(zDelta) / WHEEL_DELTA));
    return TRUE;
}

enum class KeyAction {
    Down,
    Up,
    Press,
};

static bool isKeypad(UINT virtualKeyCode, UINT keyData)
{
    switch (virtualKeyCode) {
        case VK_NUMLOCK:
        case VK_NUMPAD0:
        case VK_NUMPAD1:
        case VK_NUMPAD2:
        case VK_NUMPAD3:
        case VK_NUMPAD4:
        case VK_NUMPAD5:
        case VK_NUMPAD6:
        case VK_NUMPAD7:
        case VK_NUMPAD8:
        case VK_NUMPAD9:
        case VK_MULTIPLY:
        case VK_ADD:
        case VK_SEPARATOR:
        case VK_SUBTRACT:
        case VK_DECIMAL:
        case VK_DIVIDE:
            return true;
        case VK_RETURN:
            return keyData & KF_EXTENDED;
        case VK_INSERT:
        case VK_DELETE:
        case VK_PRIOR:
        case VK_NEXT:
        case VK_END:
        case VK_HOME:
        case VK_LEFT:
        case VK_UP:
        case VK_RIGHT:
        case VK_DOWN:
            return !(keyData & KF_EXTENDED);
        default:
            return false;
    }
}

static UINT keycodeWithLocation(UINT virtualKeyCode, UINT keyData)
{
    if (virtualKeyCode != VK_CONTROL && virtualKeyCode != VK_MENU && virtualKeyCode != VK_SHIFT)
        return virtualKeyCode;

    // If we don't need to support Windows XP or older Windows,
    // it might be better to use MapVirtualKeyEx with scancode and
    // extended keycode (i.e. 0xe0 or 0xe1).
    if (keyData & KF_EXTENDED) {
        switch (virtualKeyCode) {
        case VK_CONTROL:
            return VK_RCONTROL;
        case VK_SHIFT:
            return VK_RSHIFT;
        case VK_MENU:
            return VK_RMENU;
        default:
            break;
        }
    }

    int scancode = keyData & 0xFF;
    int regeneratedVirtualKeyCode = ::MapVirtualKey(scancode, MAPVK_VSC_TO_VK_EX);
    return regeneratedVirtualKeyCode ? regeneratedVirtualKeyCode : virtualKeyCode;
}

static bool getKeyState(UINT virtualKeyCode)
{
    return ::GetKeyState(virtualKeyCode) & 0x8000;
}

static int32_t metaStateForKeyEvent()
{
    int32_t metaState = 0;
    metaState |= getKeyState(VK_MENU) ? KeyEvent::META_ALT_ON : 0;
    metaState |= getKeyState(VK_CONTROL) ? KeyEvent::META_CTRL_ON : 0;
    metaState |= getKeyState(VK_SHIFT) ? KeyEvent::META_SHIFT_ON : 0;
    return metaState;
}

static KeyEvent keyEvent(std::chrono::milliseconds downTime, KeyAction action, UINT keyCode, UINT repeatCount, UINT keyData, bool isSystemKey)
{
    uint32_t virtualKeyCode = (action == KeyAction::Press) ? 0 : keycodeWithLocation(keyCode, keyData);
    KeyEvent event(downTime, System::currentTimeMillis(),
        (action == KeyAction::Up) ? KeyEvent::ACTION_UP : (repeatCount == 1) ? KeyEvent::ACTION_DOWN : KeyEvent::ACTION_MULTIPLE,
        VirtualKeyMap::toKeyEventKeyCode(virtualKeyCode),
        repeatCount, metaStateForKeyEvent(), 0 /* deviceId */,
        HIBYTE(HIWORD(keyData)), 0 /* flags */,
        InputDevice::SOURCE_KEYBOARD);
    KeyEventPrivate::setNativeKeyCode(event, virtualKeyCode);
    KeyEventPrivate::setVirtualKeyCode(event, virtualKeyCode);
    KeyEventPrivate::setSystemKey(event, isSystemKey);
    return event;
}

void WindowProviderWin::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    m_viewHost.dispatchKeyEvent(keyEvent(generateDownTime(nChar), KeyAction::Down, nChar, nRepCnt, nFlags, false));
}

void WindowProviderWin::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    m_viewHost.dispatchKeyEvent(keyEvent(generateDownTime(nChar), KeyAction::Down, nChar, nRepCnt, nFlags, true));
}

void WindowProviderWin::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    m_viewHost.dispatchKeyEvent(keyEvent(getDownTime(nChar), KeyAction::Up, nChar, nRepCnt, nFlags, false));
}

void WindowProviderWin::OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    m_viewHost.dispatchKeyEvent(keyEvent(getDownTime(nChar), KeyAction::Up, nChar, nRepCnt, nFlags, true));
}

void WindowProviderWin::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // Don't do anything, discard message.
}

void WindowProviderWin::OnSysChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // Don't do anything, discard message.
}

LRESULT WindowProviderWin::OnDpiChanged(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    m_viewHost.dpiChanged(HIWORD(wParam));
    return 0;
}

void WindowProviderWin::OnFinalMessage(HWND)
{
}

void WindowProviderWin::sendGeneratedMouseEnterEventIfNeeded(UINT nFlags, CPoint point)
{
    if (m_mouseEntered)
        return;

    TRACKMOUSEEVENT tme;
    tme.cbSize = sizeof(TRACKMOUSEEVENT);
    tme.dwFlags = TME_LEAVE;
    tme.hwndTrack = this->hwnd();
    if (TrackMouseEvent(&tme)) {
        m_mouseEntered = true;
        m_viewHost.dispatchMouseEvent(mouseEvent(0ms, MotionEvent::ACTION_HOVER_ENTER, pointF(point), toScreen(point), nFlags, 0, 0));
    }
}

std::chrono::milliseconds WindowProviderWin::generateDownTime(int32_t virtualKeyCode)
{
    m_downTime[virtualKeyCode] = System::currentTimeMillis();
    return m_downTime[virtualKeyCode];
}

std::chrono::milliseconds WindowProviderWin::getDownTime(int32_t virtualKeyCode)
{
    return m_downTime[virtualKeyCode];
}

// Windows IME related implementations
// Derived from Code from Webkit (https://webkit.org/) under LGPL v2 and BSD licenses (https://webkit.org/licensing-webkit/)

LRESULT WindowProviderWin::OnImeStartComposition(UINT message, WPARAM wParam, LPARAM lParam)
{
    LOGV("onIMEStartComposition");
    m_batchCount++;

    HIMC hIMC = getIMMContext();
    setCandidateWindow(hIMC);
    releaseIMMContext(hIMC);

    m_isCompositing = true;
    return TRUE;
}

LRESULT WindowProviderWin::OnImeRequest(UINT message, WPARAM wParam, LPARAM lParam)
{
    LOGV("onIMERequest wParam=%d", wParam);
    InputConnection* inputConnection = m_viewHost.inputConnection();
    if (!inputConnection)
        return TRUE;

    switch (wParam) {
        case IMR_RECONVERTSTRING:
            return onIMERequestReconvertString((RECONVERTSTRING*)lParam);

        case IMR_QUERYCHARPOSITION:
            return onIMERequestCharPosition((IMECHARPOSITION*)lParam);
    }
    return FALSE;
}

static void compositionToUnderlines(const std::vector<DWORD>& clauses, const std::vector<BYTE>& attributes, std::vector<CompositionClause>& underlines)
{
    if (clauses.empty()) {
        underlines.clear();
        return;
    }
  
    const size_t numBoundaries = clauses.size() - 1;
    underlines.resize(numBoundaries);
    for (unsigned i = 0; i < numBoundaries; i++) {
        underlines[i].startOffset = clauses[i];
        underlines[i].endOffset = clauses[i + 1];
        BYTE attribute = attributes[clauses[i]];
        underlines[i].focus = attribute == ATTR_TARGET_CONVERTED || attribute == ATTR_TARGET_NOTCONVERTED;
        underlines[i].color = 0;
    }
}

LRESULT WindowProviderWin::OnImeComposition(UINT message, WPARAM wParam, LPARAM lParam)
{
    LOGV("onIMEComposition lParam=%d", lParam);
    HIMC hIMC = getIMMContext();
    if (!hIMC)
        return TRUE;

    InputConnection* inputConnection = m_viewHost.inputConnection();
    if (!inputConnection)
        return TRUE;

    setCandidateWindow(hIMC);

    if (lParam & GCS_RESULTSTR || !lParam) {
        String compositionString;
        if (!getCompositionString(hIMC, GCS_RESULTSTR, compositionString) && lParam)
            return TRUE;

        inputConnection->commitText(compositionString, 1);
    } else {
        String compositionString;
        if (!getCompositionString(hIMC, GCS_COMPSTR, compositionString))
            return TRUE;

        // Composition string attributes
        int numAttributes = immGetCompositionString(hIMC, GCS_COMPATTR, 0, 0);
        std::vector<BYTE> attributes(numAttributes);
        immGetCompositionString(hIMC, GCS_COMPATTR, attributes.data(), numAttributes);

        // Get clauses
        int numClauses = immGetCompositionString(hIMC, GCS_COMPCLAUSE, 0, 0);
        std::vector<DWORD> clauses(numClauses / sizeof(DWORD));
        immGetCompositionString(hIMC, GCS_COMPCLAUSE, clauses.data(), numClauses);

        std::vector<CompositionClause> underlines;
        compositionToUnderlines(clauses, attributes, underlines);

        int cursorPosition = LOWORD(immGetCompositionString(hIMC, GCS_CURSORPOS, 0, 0));
        inputmethod::InputConnectionPrivate* inputConnectionPrivate = inputmethod::InputConnectionPrivate::getPrivate(*inputConnection);
        if (inputConnectionPrivate)
            inputConnectionPrivate->setComposingText(compositionString, underlines, cursorPosition);
        else
            inputConnection->setComposingText(compositionString, cursorPosition);
    }

    return TRUE;
}

LRESULT WindowProviderWin::OnImeEndComposition(UINT message, WPARAM wParam, LPARAM lParam)
{
    LOGV("onIMEEndComposition");
    InputConnection* inputConnection = m_viewHost.inputConnection();
    if (!inputConnection)
        return TRUE;

    // If the composition hasn't been confirmed yet, it needs to be resetted to zero.
    // This happens after deleting the last character from inline input hole.
    inputConnection->commitText(String(), 1);
    m_isCompositing = false;

    if (m_batchCount)
        m_batchCount--;

    return TRUE;
}

LRESULT WindowProviderWin::OnImeChar(UINT message, WPARAM wParam, LPARAM lParam)
{
    LOGV("onIMEChar U+%04X %08X", wParam, lParam);
    return TRUE;
}

LRESULT WindowProviderWin::OnImeNotify(UINT message, WPARAM wParam, LPARAM lParam)
{
    LOGV("onIMENotify wParam=%d", wParam);
    return FALSE;
}

LRESULT WindowProviderWin::OnImeSelect(UINT message, WPARAM wParam, LPARAM lParam)
{
    LOGV("onIMESelect locale %ld %s", lParam, wParam ? "select" : "deselect");
    return FALSE;
}

LRESULT WindowProviderWin::OnImeSetContext(UINT message, WPARAM wParam, LPARAM lParam)
{
    LOGV("onIMESetContext %s", wParam ? "active" : "inactive");
    return FALSE;
}

void WindowProviderWin::loadIMM()
{
    m_imm32 = ::LoadLibraryW(L"IMM32.DLL");
    immGetContext = reinterpret_cast<getContextPtr>(::GetProcAddress(m_imm32, "ImmGetContext"));
    assert(immGetContext);
    immReleaseContext = reinterpret_cast<releaseContextPtr>(::GetProcAddress(m_imm32, "ImmReleaseContext"));
    assert(immReleaseContext);
    immGetCompositionString = reinterpret_cast<getCompositionStringPtr>(::GetProcAddress(m_imm32, "ImmGetCompositionStringW"));
    assert(immGetCompositionString);
    immSetCandidateWindow = reinterpret_cast<setCandidateWindowPtr>(::GetProcAddress(m_imm32, "ImmSetCandidateWindow"));
    assert(immSetCandidateWindow);
    immSetOpenStatus = reinterpret_cast<setOpenStatusPtr>(::GetProcAddress(m_imm32, "ImmSetOpenStatus"));
    assert(immSetOpenStatus);
    immNotifyIME = reinterpret_cast<notifyIMEPtr>(::GetProcAddress(m_imm32, "ImmNotifyIME"));
    assert(immNotifyIME);
    immAssociateContextEx = reinterpret_cast<associateContextExPtr>(::GetProcAddress(m_imm32, "ImmAssociateContextEx"));
    assert(immAssociateContextEx);
}

HIMC WindowProviderWin::getIMMContext() 
{
    HIMC context = immGetContext(hwnd());
    return context;
}

void WindowProviderWin::releaseIMMContext(HIMC hIMC)
{
    if (!hIMC)
        return;

    immReleaseContext(hwnd(), hIMC);
}

void WindowProviderWin::setCandidateWindow(HIMC hIMC) 
{
    InputConnection* inputConnection = m_viewHost.inputConnection();
    if (!inputConnection)
        return;

    m_requestedCursorUpdates = true;
    if (!inputConnection->requestCursorUpdates(InputConnection::CURSOR_UPDATE_IMMEDIATE))
        return;

    assert(!m_requestedCursorUpdates);

    RectF editRect = m_cursorAnchorInfo.getCharacterBounds(m_cursorAnchorInfo.getSelectionStart());
    editRect.offsetTo(0, 0);

    CANDIDATEFORM form;
    form.dwIndex = 0;
    form.dwStyle = CFS_EXCLUDE;
    form.ptCurrentPos.x = editRect.left;
    form.ptCurrentPos.y = editRect.top + editRect.height();
    form.rcArea.top = editRect.top;
    form.rcArea.bottom = editRect.bottom;
    form.rcArea.left = editRect.left;
    form.rcArea.right = editRect.right;
    immSetCandidateWindow(hIMC, &form);
}

void WindowProviderWin::resetIME()
{
    InputConnection* inputConnection = m_viewHost.inputConnection();
    if (inputConnection && m_isCompositing) {
        inputConnection->finishComposingText();
        m_isCompositing = false;
    }

    if (HIMC hIMC = getIMMContext()) {
        immNotifyIME(hIMC, NI_COMPOSITIONSTR, CPS_CANCEL, 0);
        releaseIMMContext(hIMC);
    }
}

void WindowProviderWin::enableInputMethod(bool enabled)
{
    immAssociateContextEx(hwnd(), 0, enabled ? IACE_DEFAULT : 0);
}

void WindowProviderWin::updateCursorAnchorInfo(const CursorAnchorInfo& cursorAnchorInfo)
{
    assert(m_requestedCursorUpdates);
    m_requestedCursorUpdates = true;
    m_cursorAnchorInfo = cursorAnchorInfo;
}

bool WindowProviderWin::getCompositionString(HIMC hIMC, DWORD type, String& result)
{
    int compositionLength = immGetCompositionString(hIMC, type, 0, 0);
    if (compositionLength <= 0)
        return FALSE;
    std::vector<wchar_t> compositionBuffer(compositionLength / 2);
    compositionLength = immGetCompositionString(hIMC, type, (LPVOID)compositionBuffer.data(), compositionLength);
    result = String(compositionBuffer.data(), compositionLength / 2);
    assert(!compositionLength || compositionBuffer[0]);
    assert(!compositionLength || compositionBuffer[compositionLength / 2 - 1]);
    return TRUE;
}

LRESULT WindowProviderWin::onIMERequestCharPosition(IMECHARPOSITION* charPos)
{
    InputConnection* inputConnection = m_viewHost.inputConnection();
    if (!inputConnection)
        return FALSE;

    m_requestedCursorUpdates = true;
    if (!inputConnection->requestCursorUpdates(InputConnection::CURSOR_UPDATE_IMMEDIATE))
        return FALSE;

    assert(!m_requestedCursorUpdates);

    RectF editRect = m_cursorAnchorInfo.getCharacterBounds(m_cursorAnchorInfo.getSelectionStart());
    charPos->pt.x = editRect.left;
    charPos->pt.y = editRect.top;
    ::ClientToScreen(hwnd(), &charPos->pt);
    charPos->cLineHeight = editRect.height();
    ::GetWindowRect(hwnd(), &charPos->rcDocument);
    return TRUE;
}

LRESULT WindowProviderWin::onIMERequestReconvertString(RECONVERTSTRING* reconvertString)
{
    InputConnection* inputConnection = m_viewHost.inputConnection();
    if (!inputConnection)
        return 0;

    String text = inputConnection->getSelectedText(0);
    unsigned textSize = text.length() * sizeof(wchar_t);
    if (!reconvertString)
        return sizeof(RECONVERTSTRING) + textSize;

    unsigned totalSize = sizeof(RECONVERTSTRING) + textSize;
    if (totalSize > reconvertString->dwSize)
        return 0;
    reconvertString->dwCompStrLen = text.length();
    reconvertString->dwStrLen = text.length();
    reconvertString->dwTargetStrLen = text.length();
    reconvertString->dwStrOffset = sizeof(RECONVERTSTRING);
    memcpy(reinterpret_cast<wchar_t*>(reconvertString + 1), text.data(), textSize);
    return totalSize;
}

} // namespace view
} // namespace android
