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

namespace android {
namespace os {

class LooperHolder;

class ANDROID_EXPORT Looper {
public:
    // Returns the application's main looper, which lives in the main thread of the application.
    static Looper* getMainLooper();
    // Return the Looper object associated with the current thread.
    static Looper* myLooper();

    // Initialize the current thread as a looper, marking it as an application's main looper.
    static void prepareMainLooper();
    // Initialize the current thread as a looper.
    static void prepare();

    // Run the message queue in this thread.
    static void loop();

    // Quits the looper.
    virtual void quit();
    // Quits the looper safely.
    virtual void quitSafely();

private:
    Looper();
    ~Looper();

    static void platformLooperPrepareMain();
    static void platformLooperLoop();
    static void platformLooperQuit(int32_t);
};

} // namespace os
} // namespace android

using Looper = android::os::Looper;
