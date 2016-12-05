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

#include <android/os/Handler.h>
#include <android/os/Messenger.h>
#include <android/os/appkit/MessageFilter.h>
#include <platforms/Functional.h>

namespace android {
namespace os {
namespace appkit {

class ANDROID_EXPORT Process {
public:
    static std::unique_ptr<Process> create(const String& modulePath, const String& moduleEntry, const String& arguments,
        const std::vector<int32_t>& fileDescriptors, int32_t connectionIdentifier, intptr_t targetHandle, const std::unordered_map<String, String>& platformMainParameters);
    static Process& current();
    static void initialize();
    virtual ~Process();

    int32_t start();

    void send(Message&);
    void addMessageReceiver(std::function<bool (Messenger&, Message&)>);
    void setMessageReceiver(std::function<bool (Messenger&, Message&)>, Messages&);
    bool receive(Message&);
    bool receive(Messenger&, Message&);

    bool post(std::function<void ()>);
    bool postAtFrontOfQueue(std::function<void ()>);
    bool postAtTime(std::function<void ()>, std::chrono::milliseconds uptimeMillis);
    bool postDelayed(std::function<void ()>, std::chrono::milliseconds delayMillis);
    void removeCallbacks(std::function<void ()>);

protected:
    Process();
    Process(int32_t, IBinder);

    static Process* platformCreate(const String& modulePath, const String& moduleEntry, const String& arguments,
        const std::vector<int32_t>& fileDescriptors, int32_t connectionIdentifier, IBinder targetHandle, const std::unordered_map<String, String>& platformMainParameters);
    virtual int32_t platformStart();

    void processLaunched();

    int32_t m_connectionIdentifier;
    std::shared_ptr<Handler> m_mainThreadHandler;
    std::shared_ptr<Messenger> m_messageSender;
    std::shared_ptr<Messenger> m_messageReceiver;
    MessageFilter m_messageFilter;
};

} // namespace appkit
} // namespace os
} // namespace android

using Process = android::os::appkit::Process;
