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

#include "Process.h"

#include "ProcessLauncher.h"
#include "ProcessMessages.h"
#include <android/os/Looper.h>
#include <android/os/Messenger.h>
#include <java/lang/System.h>

#include <pthread.h>

#include <assert>

namespace android {
namespace os {
namespace appkit {

std::unique_ptr<Process> Process::create(const String& modulePath, const String& moduleEntry, const String& arguments, const std::vector<int32_t>& fileDescriptors, int32_t connectionIdentifier, intptr_t targetHandle, const std::unordered_map<String, String>& platformMainParameters)
{
    return std::unique_ptr<Process>(platformCreate(modulePath, moduleEntry, arguments, fileDescriptors, connectionIdentifier, reinterpret_cast<IBinder>(targetHandle), platformMainParameters));
}

static Process* currentProcess = nullptr;

Process& Process::current()
{
    return *currentProcess;
}

void Process::initialize()
{
    if (currentProcess)
        return;

    currentProcess = new Process();
}

static void startProcess(Process* process)
{
    assert(!currentProcess);
    currentProcess = process;

    Looper::prepareMainLooper();
    ProcessLauncher::initialize();
}

Process::Process()
    : m_connectionIdentifier(0)
    , m_mainThreadHandler(Handler::create())
    , m_messageSender(std::make_shared<Messenger>(m_mainThreadHandler))
{
    startProcess(this);
}

Process::Process(int32_t connectionIdentifier, IBinder targetHandle)
    : m_connectionIdentifier(connectionIdentifier)
    , m_mainThreadHandler(Handler::create())
    , m_messageSender(std::make_shared<Messenger>(targetHandle))
{
    startProcess(this);
}

Process::~Process()
{
    currentProcess = nullptr;
}

int32_t Process::start()
{
    assert(currentProcess);

    std::unique_ptr<std::thread> thread = std::unique_ptr<std::thread>(new std::thread([=] {
        pthread_setname_np("Process Main Thread");
        platformStart();
        post([] { Looper::myLooper()->quitSafely(); });
    }));
    thread->detach();

    Looper::loop();
    return 0;
}

class ProcessMessageHandler : public Handler {
public:
    void handleMessage(Message&) override;

    ProcessMessageHandler(Process& process) : m_process(process) { }

private:
    Process& m_process;
};

void Process::send(Message& message)
{
    m_messageSender->send(ProcessMessages::obtain(m_messageReceiver, message));
}

void Process::addMessageReceiver(std::function<bool (Messenger&, Message&)> receiver)
{
    m_messageFilter.addMessageReceiver(receiver);
}

void Process::setMessageReceiver(std::function<bool (Messenger&, Message&)> receiver, Messages& messages)
{
    m_messageFilter.setMessageReceiver(receiver, messages);
}

bool Process::receive(Message& message)
{
    if (message.what == ProcessMessages::get().LOAD_LIBRARY) {
        CharSequence libraryName = message.getData().getCharSequence(ProcessMessages::libraryNameKey());
        if (!libraryName.empty())
            System::loadLibrary(libraryName);
        return true;
    }

    return m_messageFilter.receiveMessage(*m_messageSender, message);
}

bool Process::receive(Messenger& replySender, Message& message)
{
    return m_messageFilter.receiveMessage(replySender, message);
}

bool Process::post(std::function<void ()> r)
{
    return m_mainThreadHandler->post(std::move(r));
}

bool Process::postAtFrontOfQueue(std::function<void ()> r)
{
    return m_mainThreadHandler->postAtFrontOfQueue(std::move(r));
}

bool Process::postAtTime(std::function<void ()> r, std::chrono::milliseconds uptimeMillis)
{
    return m_mainThreadHandler->postAtTime(std::move(r), uptimeMillis);
}

bool Process::postDelayed(std::function<void ()> r, std::chrono::milliseconds delayMillis)
{
    return m_mainThreadHandler->postDelayed(std::move(r), delayMillis);
}

void Process::removeCallbacks(std::function<void ()> r)
{
    m_mainThreadHandler->removeCallbacks(std::move(r));
}

int32_t Process::platformStart()
{
    return 0;
}

void Process::processLaunched()
{
    m_messageReceiver = std::make_shared<Messenger>(std::make_shared<ProcessMessageHandler>(*this));
    m_messageSender->send(ProcessMessages::get().ProcessLaunched(m_messageReceiver, m_connectionIdentifier));
}

void ProcessMessageHandler::handleMessage(Message& message)
{
    m_process.receive(message);
}

} // namespace appkit
} // namespace os
} // namespace android
