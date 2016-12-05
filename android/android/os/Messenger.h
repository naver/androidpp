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

#include <android/os/IBinder.h>

namespace android {
namespace os {

class Handler;
class Message;
class MessageTarget;

class Messenger final {
public:
    // Create a new Messenger pointing to the given Handler.
    ANDROID_EXPORT Messenger(IBinder);
    // Create a Messenger from a raw IBinder, which had previously been retrieved with getBinder().
    ANDROID_EXPORT Messenger(std::shared_ptr<Handler>);
    ANDROID_EXPORT ~Messenger();

    // Send a Message to this Messenger's Handler.
    ANDROID_EXPORT void send(Message&);

    // Retrieve the IBinder that this Messenger is using to communicate with its associated Handler.
    ANDROID_EXPORT IBinder getBinder();

private:
    std::unique_ptr<MessageTarget> m_target;
};

} // namespace os
} // namespace android

using Messenger = android::os::Messenger;
