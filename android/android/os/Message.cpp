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

#include "Message.h"

#include "Bundle.h"
#include "Messenger.h"

namespace android {
namespace os {

Message::Message()
    : what(0)
    , arg1(0)
    , arg2(0)
    , obj(0)
    , target(0)
    , replyTo(0)
    , data(nullptr)
{
}

Message::Message(const Message& o)
    : what(o.what)
    , arg1(o.arg1)
    , arg2(o.arg2)
    , obj(o.obj)
    , target(o.target)
    , replyTo(o.replyTo)
    , data((o.data) ? new Bundle(*o.data) : nullptr)
{
}

Message::Message(Message&& o)
    : what(o.what)
    , arg1(o.arg1)
    , arg2(o.arg2)
    , obj(o.obj)
    , target(o.target)
    , replyTo(std::move(o.replyTo))
    , data(o.data)
{
    o.data = nullptr;
}

Message::~Message()
{
    if (data)
        delete data;
}

Message Message::obtain()
{
    return Message();
}

Message Message::obtain(Handler::ptr_t h)
{
    Message m;
    m.target = h;
    return m;
}

Message Message::obtain(Handler::ptr_t h, int32_t what)
{
    Message m;
    m.what = what;
    m.target = h;
    return m;
}

Message Message::obtain(Handler::ptr_t h, int32_t what, int32_t arg1, int32_t arg2)
{
    Message m;
    m.what = what;
    m.arg1 = arg1;
    m.arg2 = arg2;
    m.target = h;
    return m;
}

Message Message::obtain(Handler::ptr_t h, int32_t what, intptr_t obj)
{
    Message m;
    m.what = what;
    m.obj = obj;
    m.target = h;
    return m;
}

Message Message::obtain(Handler::ptr_t h, int32_t what, int32_t arg1, int32_t arg2, intptr_t obj)
{
    Message m;
    m.what = what;
    m.arg1 = arg1;
    m.arg2 = arg2;
    m.obj = obj;
    m.target = h;
    return m;
}

Message Message::obtain(const Message& orig)
{
    Message m;
    m.what = orig.what;
    m.arg1 = orig.arg1;
    m.arg2 = orig.arg2;
    m.obj = orig.obj;
    m.target = orig.target;
    m.replyTo = orig.replyTo;
    m.data = (orig.data) ? new Bundle(*orig.data) : nullptr;
    return m;
}

void Message::setData(Bundle& data)
{
    if (this->data)
        delete this->data;

    this->data = new Bundle(data);
}

void Message::setData(Bundle&& data)
{
    if (this->data)
        delete this->data;

    this->data = new Bundle(data);
}

Bundle& Message::getData()
{
    if (!data)
        data = new Bundle;

    return *data;
}

Bundle* Message::peekData()
{
    return data;
}

Message& Message::operator=(Message&& other)
{
    what = other.what;
    arg1 = other.arg1;
    arg2 = other.arg2;
    obj = other.obj;
    target = other.target;
    replyTo = std::move(other.replyTo);
    data = other.data;
    other.data = nullptr;
    return *this;
}

} // namespace os
} // namespace android
