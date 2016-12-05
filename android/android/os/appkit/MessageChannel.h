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

#pragma once

#include <android/os/appkit/Messages.h>

namespace android {
namespace os {
namespace appkit {

class MessageChannel : private Object {
public:
    ANDROID_EXPORT virtual ~MessageChannel();

protected:
    ANDROID_EXPORT MessageChannel();

    ANDROID_EXPORT void send(Message&);
    ANDROID_EXPORT void send(Message&, int32_t waitFor);
    ANDROID_EXPORT void post(std::function<void ()>&&, int32_t waitFor);
    ANDROID_EXPORT virtual void receive(const Message&);
    ANDROID_EXPORT void wait();

    ANDROID_EXPORT void connect(int32_t);
    ANDROID_EXPORT void disconnect();

    int32_t channelIdentifier() { return m_channelIdentifier; }

private:
    static void receiveMessage(MessageChannel&, Message&);

    int32_t m_channelIdentifier;
    std::shared_ptr<Messenger> m_messageSender;
    std::shared_ptr<Messenger> m_messageReceiver;

    bool m_threadLock;
    int32_t m_threadUnlocker;
};

} // namespace appkit
} // namespace os
} // namespace android

using MessageChannel = android::os::appkit::MessageChannel;
