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

#include "ProcessLauncher.h"

#include "Looper.h"
#include "MessageHost.h"
#include "Messenger.h"
#include "Process.h"
#include "ProcessMessages.h"
#include "Thread.h"
#include <java/lang/System.h>

#include <unordered_map>
#include <unordered_set>

#include <assert>

namespace android {
namespace os {
namespace appkit {

static std::unordered_set<int32_t> runningProcesses;
static std::unordered_map<int32_t, std::shared_ptr<ProcessLauncher::Connection>> processConnections;
static std::unordered_map<Messenger*, int32_t> openProcesses;

class ProcessLauncherMessageHandler : public Handler {
public:
    void handleMessage(Message&) override;

    ProcessLauncherMessageHandler(std::function<void (Message&)> handler)
        : m_handler(std::move(handler)) { }

    std::function<void (Message&)> m_handler;
};

std::shared_ptr<Messenger> ProcessLauncher::m_messageReceiver;
std::function<void (int32_t)> ProcessLauncher::m_processLaunchedListener;

void ProcessLauncher::initialize()
{
    assert(Thread::isMainThread());

    m_messageReceiver = std::make_shared<Messenger>(std::make_shared<ProcessLauncherMessageHandler>([] (Message& message) { handleMessage(message); }));

    platformInitialize();
}

void ProcessLauncher::setProcessLaunchedListener(std::function<void (int32_t)> listener)
{
    m_processLaunchedListener = std::move(listener);
}

bool ProcessLauncher::connect(String& moduleName, String& moduleEntry, String& arguments, const std::vector<int32_t>& fileDescriptors, std::function<void (intptr_t)> connectionCallback)
{
    Connection* connection = platformCreateProcess(moduleName, moduleEntry, arguments, fileDescriptors);
    if (!connection)
        return false;

    int32_t connectionIdentifier = connection->connectionIdentifier();
    runningProcesses.insert(connectionIdentifier);
    processConnections[connectionIdentifier] = std::shared_ptr<Connection>(connection);

    connectionCallback(connectionIdentifier);
    return true;
}

std::shared_ptr<ProcessLauncher::Connection> ProcessLauncher::getConnection(int32_t connection)
{
    if (processConnections.count(connection) == 0)
        return nullptr;

    return processConnections[connection];
}

bool ProcessLauncher::isConnectedToProcess(int32_t connection)
{
    return runningProcesses.count(connection) != 0;
}

bool ProcessLauncher::isProcessDisconnected(int32_t connection)
{
    return runningProcesses.count(connection) == 0;
}

void ProcessLauncher::unbindProcess(int32_t connection)
{
    if (!isConnectedToProcess(connection))
        return;

    runningProcesses.erase(connection);

    std::shared_ptr<ProcessLauncher::Connection> processConnection = processConnections[connection];
    if (processConnection->m_messageSender)
        openProcesses.erase(processConnection->m_messageSender.get());
    processConnections.erase(connection);
    processConnection->unbind();
}

void ProcessLauncher::handleMessage(Message& message)
{
    if (message.what == ProcessMessages::get().PROCESS_LAUNCHED) {
        int32_t connectionIdentifier = message.arg1;
        std::shared_ptr<ProcessLauncher::Connection> connection = processConnections[connectionIdentifier];
        if (!connection)
            return;
        connection->m_messageSender.reset(message.replyTo);
        openProcesses[message.replyTo] = connectionIdentifier;
        if (m_processLaunchedListener)
            m_processLaunchedListener(connectionIdentifier);
        return;
    }

    if (!openProcesses.count(message.replyTo)) {
        Process::current().receive(message);
        return;
    }

    int32_t connectionIdentifier = openProcesses[message.replyTo];
    std::shared_ptr<ProcessLauncher::Connection> connection = ProcessLauncher::getConnection(connectionIdentifier);
    if (!connection) {
        // ERROR: Message has no valid destination!
        return;
    }

    if (connection->receive(message))
        return;

    Process::current().receive(*connection->m_messageSender, message);
}

static int32_t uniqueConnectionIdentifier()
{
    static int32_t connectionIdentifier = 0;
    return ++connectionIdentifier;
}

ProcessLauncher::Connection::Connection()
    : m_connectionIdentifier(uniqueConnectionIdentifier())
{
}

void ProcessLauncher::Connection::send(Message& message)
{
    m_messageSender->send(message);
}

void ProcessLauncher::Connection::addMessageReceiver(std::function<bool (Messenger&, Message&)> receiver)
{
    m_messageFilter.addMessageReceiver(receiver);
}

void ProcessLauncher::Connection::setMessageReceiver(std::function<bool (Messenger&, Message&)> receiver, Messages& messages)
{
    m_messageFilter.setMessageReceiver(receiver, messages);
}

bool ProcessLauncher::Connection::receive(Message& message)
{
    return m_messageFilter.receiveMessage(*m_messageSender, message);
}

inline void ProcessLauncherMessageHandler::handleMessage(Message& message)
{
    m_handler(message);
}

} // namespace appkit
} // namespace os
} // namespace android
