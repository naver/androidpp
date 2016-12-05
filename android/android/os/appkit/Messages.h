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

#include <android/os/Message.h>
#include <android/os/Messenger.h>

namespace android {
namespace os {
namespace appkit {

class Messages {
public:
    static Message obtain(std::shared_ptr<Messenger>& replyTo, int32_t what,
            int32_t arg1 = 0, int32_t arg2 = 0)
    {
        Message message = Message::obtain(nullptr, what, arg1, arg2);
        message.replyTo = replyTo.get();
        return message;
    }

    static Message obtain(std::shared_ptr<Messenger>& replyTo, Message& m)
    {
        Message message = Message::obtain(nullptr, m.what, m.arg1, m.arg2, m.obj);
        message.replyTo = (m.replyTo) ? m.replyTo : replyTo.get();
        return message;
    }

    static inline int32_t identifyMessages(int32_t what)
    {
        return static_cast<int32_t>(what & (0xFFFFFFFF << 20));
    }

    inline int32_t getMessagesIdentifier()
    {
        return m_messagesIdentifier;
    }

protected:
    Messages()
        : m_messagesIdentifier(++m_registeredMessages << 20)
    {
    }

    inline int32_t getUniqueMessageIdentifier(int32_t id)
    {
        return m_messagesIdentifier | id;
    }

private:
    int32_t m_messagesIdentifier;
    static int32_t m_registeredMessages;
};

} // namespace appkit
} // namespace os
} // namespace android

using Messages = android::os::appkit::Messages;
