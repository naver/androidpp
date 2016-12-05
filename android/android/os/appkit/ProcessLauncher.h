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

#include <android/os/appkit/MessageFilter.h>

namespace android {
namespace os {
namespace appkit {

class ProcessLauncher final {
public:
    class Connection {
        friend class ProcessLauncher;
    public:
        ANDROID_EXPORT virtual ~Connection() = default;

        ANDROID_EXPORT virtual intptr_t processIdentifier() const = 0;
        ANDROID_EXPORT virtual void unbind() = 0;
        int32_t connectionIdentifier() { return m_connectionIdentifier; }
        ANDROID_EXPORT void send(Message&);
        ANDROID_EXPORT void addMessageReceiver(std::function<bool (Messenger&, Message&)>);
        ANDROID_EXPORT void setMessageReceiver(std::function<bool (Messenger&, Message&)>, Messages&);
        ANDROID_EXPORT bool receive(Message&);

    protected:
        Connection();

        int32_t m_connectionIdentifier;
        std::shared_ptr<Messenger> m_messageSender;
        MessageFilter m_messageFilter;
    };

    static const intptr_t invalidHandleValue;

    ANDROID_EXPORT static void initialize();

    ANDROID_EXPORT static void setProcessLaunchedListener(std::function<void (int32_t)>);

    ANDROID_EXPORT static bool connect(String& moduleName, String& moduleEntry, String& arguments,
        const std::vector<int32_t>& fileDescriptors, std::function<void (intptr_t)> connectionCallback);

    ANDROID_EXPORT static std::shared_ptr<Connection> getConnection(int32_t);

    ANDROID_EXPORT static bool isConnectedToProcess(int32_t);
    ANDROID_EXPORT static bool isProcessDisconnected(int32_t);
    ANDROID_EXPORT static void unbindProcess(int32_t);

private:
    ProcessLauncher() = default;
    ~ProcessLauncher() = default;

    static void handleMessage(Message& message);

    static void platformInitialize();
    static Connection* platformCreateProcess(const String& moduleName, const String& moduleEntry,
        const String& arguments, const std::vector<int32_t>& fileDescriptors);

    static std::shared_ptr<Messenger> m_messageReceiver;
    static std::function<void (int32_t)> m_processLaunchedListener;
};

} // namespace appkit
} // namespace os
} // namespace android

using ProcessLauncher = android::os::appkit::ProcessLauncher;
