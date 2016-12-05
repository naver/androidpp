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

class MessageChannelMessages : public Messages {
public:
    const int32_t CONNECTED;
    const int32_t DISCONNECTED;

    static MessageChannelMessages& get()
    {
        static MessageChannelMessages shared;
        return shared;
    }

    inline Message Connected(int32_t channel, std::shared_ptr<Messenger>& replyTo)
    {
        return Messages::obtain(replyTo, CONNECTED, channel);
    }

    inline Message Disconnected(int32_t channel)
    {
        return Message::obtain(0, DISCONNECTED, channel, 0);
    }

private:
    MessageChannelMessages()
        : CONNECTED(getUniqueMessageIdentifier(0))
        , DISCONNECTED(getUniqueMessageIdentifier(1))
    {
    }
};

} // namespace appkit
} // namespace os
} // namespace android

using MessageChannelMessages = android::os::appkit::MessageChannelMessages;
