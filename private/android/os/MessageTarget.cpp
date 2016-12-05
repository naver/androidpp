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

#include <android/os/Handler.h>

#include <assert>

namespace android {
namespace os {

class HandlerMessageTarget : public MessageTarget {
public:
    HandlerMessageTarget(std::shared_ptr<Handler> target);
    ~HandlerMessageTarget();

    void send(Message&) override;

    IBinder handle() const override;

private:
    std::shared_ptr<Handler> m_target;
};

HandlerMessageTarget::HandlerMessageTarget(std::shared_ptr<Handler> target)
    : m_target(target)
{
}

HandlerMessageTarget::~HandlerMessageTarget()
{
}

void HandlerMessageTarget::send(Message& message)
{
    m_target->sendMessage(message);
}

IBinder HandlerMessageTarget::handle() const
{
    return platformGetHandlerHandle(*m_target);
}

std::unique_ptr<MessageTarget> MessageTarget::create(std::shared_ptr<Handler> target)
{
    return std::unique_ptr<MessageTarget>(new HandlerMessageTarget(target));
}

} // namespace os
} // namespace android
