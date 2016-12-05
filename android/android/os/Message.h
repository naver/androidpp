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

#include <android/os/Bundle.h>
#include <android/os/Handler.h>

namespace android {
namespace os {

class Messenger;

class ANDROID_EXPORT Message final {
public:
    int32_t what;
    int32_t arg1;
    int32_t arg2;
    intptr_t obj;
    Handler::ptr_t target;
    Messenger* replyTo;

    Message();
    Message(const Message&);
    Message(Message&&);
    ~Message();

    // FIXME: Implement a global pool for obtain()s.
    // Return a new Message instance from the global pool.
    static Message obtain();

    // Same as obtain(), but sets the value for the target member on the Message returned.
    static Message obtain(Handler::ptr_t h);
    // Same as obtain(), but sets the values for both target and what members on the Message.
    static Message obtain(Handler::ptr_t h, int32_t what);
    // Same as obtain(), but sets the values of the target, what, arg1, and arg2 members.
    static Message obtain(Handler::ptr_t h, int32_t what,
            int32_t arg1, int32_t arg2);
    // Same as obtain(), but sets the values of the target, what, and obj members.
    static Message obtain(Handler::ptr_t h, int32_t what, intptr_t obj);
    // Same as obtain(), but sets the values of the target, what, arg1, arg2, and obj members.
    static Message obtain(Handler::ptr_t h, int32_t what,
            int32_t arg1, int32_t arg2, intptr_t obj);

    // Same as obtain(), but copies the values of an existing message (including its target) into the new one.
    static Message obtain(const Message& orig);

    // Sets a Bundle of arbitrary data values. 
    void setData(Bundle& data);
    void setData(Bundle&& data);
    // Obtains a Bundle of arbitrary data associated with this event, lazily creating it if necessary. 
    Bundle& getData();
    // Like getData(), but does not lazily create the Bundle. 
    Bundle* peekData();

    Message& operator=(Message&&);

private:
    mutable Bundle* data;
};

} // namespace os
} // namespace android

using Message = android::os::Message;
