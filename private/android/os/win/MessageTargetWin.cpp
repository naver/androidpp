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

#include "MessageTarget.h"

#include "HandlerProviderWin.h"
#include "MessageCopyData.h"
#include <android/os/Message.h>
#include <android/os/Messenger.h>

#include <array>

#include <assert>

namespace android {
namespace os {

class DataCopyWindowMessageTarget : public MessageTarget {
public:
    DataCopyWindowMessageTarget(IBinder target);
    ~DataCopyWindowMessageTarget();

    void send(Message&) override;

    IBinder handle() const override;

private:
    HWND m_target;
    DWORD m_processIdentifier;
};

DataCopyWindowMessageTarget::DataCopyWindowMessageTarget(IBinder target)
    : m_target(reinterpret_cast<HWND>(target))
    , m_processIdentifier(::GetCurrentProcessId())
{
    assert(target);
}

DataCopyWindowMessageTarget::~DataCopyWindowMessageTarget()
{
}

void DataCopyWindowMessageTarget::send(Message& message)
{
    COPYDATASTRUCT data;
    MessageCopyData::Data values;
    data.dwData = (ULONG_PTR)m_processIdentifier;
    data.cbData = MessageCopyData::pack(values, message);
    data.lpData = (PVOID)values.data();
    while (!::SendMessageTimeoutA(m_target, WM_COPYDATA, (WPARAM)message.replyTo->getBinder(), (LPARAM)&data,
        SMTO_ABORTIFHUNG | SMTO_NOTIMEOUTIFNOTHUNG | SMTO_ERRORONEXIT, 1, NULL)) {
        DWORD lastError = ::GetLastError();
        if (lastError == ERROR_TIMEOUT || !lastError)
            continue;
        if (lastError == ERROR_INVALID_HANDLE)
            // FIXME: This seems strange, but a message sending could fail with ERROR_INVALID_HANDLE even if the targe window handle is valid.
            // FIXME: Figure out whether the target window is actually invalid or not.
            continue;
        // FIXME: Do something!
        assert(false);
    }
}

IBinder DataCopyWindowMessageTarget::handle() const
{
    return reinterpret_cast<IBinder>(m_target);
}

std::unique_ptr<MessageTarget> MessageTarget::create(IBinder target)
{
    return std::unique_ptr<MessageTarget>(new DataCopyWindowMessageTarget(target));
}

IBinder MessageTarget::platformGetHandlerHandle(Handler& handler)
{
    return reinterpret_cast<IBinder>(HandlerProviderWin::messageWindowHandle(*handler.m_handler));
}

} // namespace os
} // namespace android
