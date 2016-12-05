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

#include <java/lang.h>

#include <vector>

namespace android {
namespace os {

class HandlerProvider;
class Looper;
class Message;
class MessageTarget;
class WorkItem;

class ANDROID_EXPORT Handler : public Object {
    friend class HandlerProvider;
    friend class MessageTarget;
public:
    typedef Handler* ptr_t;

    static std::shared_ptr<Handler> create();
    virtual ~Handler();

    Looper* getLooper();

    // Subclasses must implement this to receive messages.
    virtual void handleMessage(Message& msg);

    // Check if there are any pending posts of messages with code 'what' in the message queue.
    bool hasMessages(int32_t what);

    // Returns a new Message from the global message pool.
    Message obtainMessage();
    // Same as obtainMessage();, except that it also sets the what member of the returned Message.
    Message obtainMessage(int32_t what);
    // Same as obtainMessage();, except that it also sets the what, arg1 and arg2 members of the returned Message.
    Message obtainMessage(int32_t what, int32_t arg1, int32_t arg2);

    // Remove any pending posts of messages with code 'what' that are in the message queue.
    void removeMessages(int32_t what);

    // Causes the std::function<void ()> r to be added to the message queue.
    bool post(std::function<void ()> r);
    // Posts a message to an object that implements std::function<void ()>.
    bool postAtFrontOfQueue(std::function<void ()> r);
    // Causes the std::function<void ()> r to be added to the message queue, to be run at a specific time given by uptimeMillis.
    bool postAtTime(std::function<void ()> r, std::chrono::milliseconds uptimeMillis);
    // Causes the std::function<void ()> r to be added to the message queue, to be run after the specified amount of time elapses.
    bool postDelayed(std::function<void ()> r, std::chrono::milliseconds delayMillis);

    // Remove any pending posts of std::function<void ()> r that are in the message queue.
    void removeCallbacks(std::function<void ()> r);

    // Sends a Message containing only the what value.
    bool sendEmptyMessage(int32_t what);
    // Sends a Message containing only the what value, to be delivered at a specific time.
    bool sendEmptyMessageAtTime(int32_t what, std::chrono::milliseconds uptimeMillis);
    // Sends a Message containing only the what value, to be delivered after the specified amount of time elapses.
    bool sendEmptyMessageDelayed(int32_t what, std::chrono::milliseconds delayMillis);

    // Pushes a message onto the end of the message queue after all pending messages before the current time.
    bool sendMessage(Message& msg);
    // Enqueue a message at the front of the message queue, to be processed on the next iteration of the message loop.
    bool sendMessageAtFrontOfQueue(Message& msg);
    // Enqueue a message into the message queue after all pending messages before the absolute time (in milliseconds); uptimeMillis.
    virtual bool sendMessageAtTime(Message& msg, std::chrono::milliseconds uptimeMillis);
    // Enqueue a message into the message queue after all pending messages before (current time + delayMillis);.
    bool sendMessageDelayed(Message& msg, std::chrono::milliseconds delayMillis);

protected:
    Handler();

private:
    template<typename... Arguments> void removeWorkItems(Arguments...);

    bool start();
    bool startAtTime();
    void stop();

    // HandlerProvider
    void performMessages();
    void receivedMessage(Message&);

    std::unique_ptr<HandlerProvider> m_handler;
    std::vector<std::unique_ptr<WorkItem>> m_workQueue;
    std::chrono::milliseconds m_nextFireTime;
    Looper* m_looper;
};

} // namespace os
} // namespace android

using Handler = android::os::Handler;
