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

#include "MessageHost.h"

#include "MessageChannelMessages.h"
#include "MessageHostMessages.h"
#include "Process.h"
#include "Thread.h"

#include <unordered_map>
#include <mutex>

#include <assert>

namespace android {
namespace os {
namespace appkit {

static int32_t uniqueChannelIdentifier()
{
    static int32_t channelIdentifier = 0;
    return ++channelIdentifier;
}

static Object staticMutex;
static std::unordered_map<int32_t, std::shared_ptr<MessageHost>> openChannels;
static std::unordered_map<int32_t, std::shared_ptr<MessageHost>> disconnectedChannels;

class MessageHostHandler : public Handler {
public:
    MessageHostHandler(MessageHost& channel, std::function<void (MessageHost&, Message&)> r)
        : m_channel(channel)
        , m_receiver(std::move(r))
    { }

    void handleMessage(Message& message) override { m_receiver(m_channel, message); }

private:
    MessageHost& m_channel;
    std::function<void (MessageHost&, Message&)> m_receiver;
};

std::shared_ptr<MessageHost> MessageHost::get(int32_t channelIdentifier)
{
    synchronized (staticMutex) {
        if (!openChannels.count(channelIdentifier)) {
            if (disconnectedChannels.count(channelIdentifier)) {
                std::shared_ptr<MessageHost> channel = disconnectedChannels[channelIdentifier];
                disconnectedChannels.erase(channelIdentifier);
                return std::move(channel);
            }
            return nullptr;
        }

        std::shared_ptr<MessageHost> channel = openChannels[channelIdentifier];
        channel->m_published = true;
        return std::move(channel);
    }
}

MessageHost::MessageHost(std::shared_ptr<Messenger>&& messageSender)
    : m_channelIdentifier(uniqueChannelIdentifier())
    , m_messageSender(messageSender)
    , m_messageReceiver(std::make_shared<Messenger>(std::make_shared<MessageHostHandler>(*this, receiveMessage)))
    , m_protected(false)
    , m_published(false)
{
    connected();
}

MessageHost::~MessageHost()
{
}

void MessageHost::protect()
{
    m_protected = true;
}

void MessageHost::send(Message& message)
{
    std::shared_ptr<Messenger> messageSender = m_messageSender;
    if (!messageSender)
        return;

    Thread::runOnMainThread([=] {
        messageSender->send(Message::obtain(message));
    });
}

void MessageHost::receive(Message& message)
{
    if (message.what == MessageHostMessages::get().DISCONNECT) {
        disconnected();
    } else {
        assert(false);
    }
}

void MessageHost::connected()
{
    assert(Thread::isMainThread());
    assert(!openChannels.count(m_channelIdentifier));

    synchronized (staticMutex) {
        openChannels[m_channelIdentifier] = std::shared_ptr<MessageHost>(this);
    }

    m_messageSender->send(MessageChannelMessages::get().Connected(m_channelIdentifier, m_messageReceiver));
}

void MessageHost::disconnected()
{
    assert(Thread::isMainThread());
    assert(openChannels.count(m_channelIdentifier) > 0);

    m_messageReceiver.reset();

    std::shared_ptr<Messenger> messageSender = m_messageSender;
    if (!messageSender)
        return;

    m_messageSender.reset();

    int32_t channelIdentifier = m_channelIdentifier;

    Thread::runOnMainThread([=] {
        messageSender->send(MessageChannelMessages::get().Disconnected(channelIdentifier));
        synchronized (staticMutex) {
            std::shared_ptr<MessageHost> channel = openChannels[channelIdentifier];
            if (channel->m_protected && !channel->m_published)
                disconnectedChannels[channelIdentifier] = channel;
            openChannels.erase(channelIdentifier);
        }
    });
}

void MessageHost::receiveMessage(MessageHost& channel, Message& message)
{
    return channel.receive(message);
}

} // namespace appkit
} // namespace os
} // namespace android
