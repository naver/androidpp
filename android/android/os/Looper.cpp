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

#include "Looper.h"

#include <assert>
#include <pthread.h>

namespace android {
namespace os {

Looper::Looper()
{
}

Looper::~Looper()
{
}

static Looper* mainLooper;
extern pthread_main_np_t mainThreadIdentifier; // Thread.cpp

// Returns the application's main looper, which lives in the main thread of the application.
Looper* Looper::getMainLooper()
{
    if (!mainLooper)
        assert(0);

    return mainLooper;
}

static thread_local Looper* threadLooper;

// Return the Looper object associated with the current thread.
Looper* Looper::myLooper()
{
    return threadLooper;
}

// Initialize the current thread as a looper, marking it as an application's main looper.
void Looper::prepareMainLooper()
{
    if (mainLooper)
        return;

    prepare();
    pthread_init_main_np();

    mainLooper = threadLooper;
    mainThreadIdentifier = pthread_get_main_np();

    platformLooperPrepareMain();
}

// Initialize the current thread as a looper.
void Looper::prepare()
{
    if (myLooper())
        return;

    threadLooper = new Looper();
}

// Run the message queue in this thread.
void Looper::loop()
{
    assert(threadLooper);

    platformLooperLoop();

    delete threadLooper;
}

// Quits the looper.
void Looper::quit()
{
    platformLooperQuit(0);
}

// Quits the looper safely.
void Looper::quitSafely()
{
    platformLooperQuit(0);
}

} // namespace os
} // namespace android
