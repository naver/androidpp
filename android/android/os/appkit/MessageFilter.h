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
#include <platforms/Functional.h>

#include <unordered_map>

namespace android {
namespace os {
namespace appkit {

class MessageFilter {
public:
    void addMessageReceiver(std::function<bool (Messenger&, Message&)>);
    void setMessageReceiver(std::function<bool (Messenger&, Message&)>, Messages&);

    bool receiveMessage(Messenger&, Message&);

private:
    std::vector<std::safe_function<bool (Messenger&, Message&)>> m_messageReceivers;
    std::unordered_map<int32_t, std::safe_function<bool (Messenger&, Message&)>> m_dedicatedMessageReceivers;
};

inline void MessageFilter::addMessageReceiver(std::function<bool (Messenger&, Message&)> receiver)
{
    m_messageReceivers.push_back(std::move(receiver));
}

inline void MessageFilter::setMessageReceiver(std::function<bool (Messenger&, Message&)> receiver, Messages& messages)
{
    m_dedicatedMessageReceivers[messages.getMessagesIdentifier()] = std::move(receiver);
}

inline bool MessageFilter::receiveMessage(Messenger& replySender, Message& message)
{
    int32_t messages = Messages::identifyMessages(message.what);
    if (m_dedicatedMessageReceivers.count(messages) > 0)
        if (m_dedicatedMessageReceivers[messages](replySender, message))
            return true;

    for (auto& messageHandler : m_messageReceivers)
        if (messageHandler(replySender, message))
            return true;

    return false;
}

} // namespace appkit
} // namespace os
} // namespace android

using MessageFilter = android::os::appkit::MessageFilter;
