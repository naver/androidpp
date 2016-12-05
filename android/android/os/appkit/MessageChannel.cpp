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

#include "MessageChannel.h"

#include "MessageChannelMessages.h"
#include "MessageHostMessages.h"
#include "Process.h"
#include "ProcessMessages.h"
#include "Thread.h"

#include <mutex>

#include <assert>

namespace android {
namespace os {
namespace appkit {

class MessageChannelHandler : public Handler {
public:
    MessageChannelHandler(MessageChannel& channel, std::function<void (MessageChannel&, Message&)> r)
        : m_channel(channel)
        , m_receiver(std::move(r))
    { }

    void handleMessage(Message& message) override { m_receiver(m_channel, message); }

private:
    MessageChannel& m_channel;
    std::function<void (MessageChannel&, Message&)> m_receiver;
};

MessageChannel::MessageChannel()
    : m_channelIdentifier(0)
    , m_threadLock(false)
    , m_threadUnlocker(0)
{
}

MessageChannel::~MessageChannel()
{
    disconnect();
}

void MessageChannel::send(Message& message)
{
    std::shared_ptr<Messenger> messageSender = m_messageSender;
    if (!messageSender)
        return;

    Thread::runOnMainThread([=] {
        messageSender->send(Message::obtain(message));
    });
}

void MessageChannel::send(Message& message, int32_t waitFor)
{
    assert(!m_threadLock);

    synchronized (this) {
        m_threadUnlocker = waitFor;
        send(message);
        m_threadLock = true;
        while (m_threadLock)
            Lockable::wait();
    }
}

void MessageChannel::post(std::function<void ()>&& runnable, int32_t waitFor)
{
    assert(!m_threadLock);

    synchronized (this) {
        m_threadUnlocker = waitFor;
        Thread::runOnMainThread([=] { runnable(); });
        m_threadLock = true;
        while (m_threadLock)
            Lockable::wait();
    }
}

void MessageChannel::receive(const Message& message)
{
    if (message.what == MessageChannelMessages::get().CONNECTED) {
        m_channelIdentifier = message.arg1;
        m_messageSender.reset(message.replyTo);
    } else if (message.what == MessageChannelMessages::get().DISCONNECTED) {
        m_messageReceiver.reset();
    } else {
        assert(false);
    }
}

void MessageChannel::wait()
{
    assert(!Thread::isMainThread());
    assert(!m_threadLock);
    assert(!m_threadUnlocker);

    synchronized (this) {
        m_threadLock = true;
        while (m_threadLock)
            Lockable::wait();
    }
}

void MessageChannel::connect(int32_t what)
{
    assert(!m_messageReceiver);

    post([=] {
        m_messageReceiver = std::make_shared<Messenger>(std::make_shared<MessageChannelHandler>(*this, receiveMessage));
        Process::current().send(ProcessMessages::obtain(m_messageReceiver, what));
    }, MessageChannelMessages::get().CONNECTED);
}

void MessageChannel::disconnect()
{
    std::shared_ptr<Messenger> messageSender = m_messageSender;
    if (!messageSender)
        return;

    m_messageSender.reset();

    int32_t channelIdentifier = m_channelIdentifier;

    post([=] {
        messageSender->send(MessageHostMessages::get().Disconnect(channelIdentifier));
    }, MessageChannelMessages::get().DISCONNECTED);
}

void MessageChannel::receiveMessage(MessageChannel& channel, Message& message)
{
    synchronized (channel) {
        channel.receive(message);
        if (channel.m_threadLock) {
            if (channel.m_threadUnlocker &&
                channel.m_threadUnlocker != message.what)
                return;
            channel.m_threadLock = false;
            channel.notifyAll();
        }
    }
}

} // namespace appkit
} // namespace os
} // namespace android
