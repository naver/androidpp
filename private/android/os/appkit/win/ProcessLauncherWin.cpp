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

#include <android/os/appkit/ProcessLauncher.h>

#include <android/os/Messenger.h>
#include <android/os/win/HandlerProviderWin.h>
#include <java/lang/System.h>
#include <platforms/StringConversion.h>

#include <VersionHelpers.h>
#include <Windows.h>
#include <shlwapi.h>

#include <assert>
#include <mutex>
#include <string>
#include <sys/socket.h>

namespace android {
namespace os {
namespace appkit {

const intptr_t ProcessLauncher::invalidHandleValue = reinterpret_cast<intptr_t>(INVALID_HANDLE_VALUE);

static const char* processJobName()
{
    static char name[MAX_PATH];
    _snprintf(name, MAX_PATH, "os.ProcessLauncher.Job.%ld", ::GetCurrentProcessId());
    return name;
}

class JobObject {
public:
    ~JobObject();

    static HANDLE handle()
    {
        static JobObject jobObject;
        return jobObject.m_jobObject;
    }

private:
    JobObject();

    HANDLE m_jobObject;
};

JobObject::JobObject()
    : m_jobObject(::CreateJobObjectA(NULL, processJobName()))
{
    // Causes all processes associated with the job to terminate when the
    // last handle to the job is closed.        
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli = { 0 };
    jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    if (!IsWindows8OrGreater()) {
        // Windows 8 implements nested jobs, but for older systems we need to
        // break out of any job we're in to enforce our restrictions.
        jeli.BasicLimitInformation.LimitFlags |= JOB_OBJECT_LIMIT_BREAKAWAY_OK;
    }

    ::SetInformationJobObject(m_jobObject, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli));
}

JobObject::~JobObject()
{
    ::CloseHandle(m_jobObject);
}

class ProcessConnection : public ProcessLauncher::Connection {
public:
    ProcessConnection();
    ~ProcessConnection();

    intptr_t processIdentifier() const { return m_processIdentifier; }
    void unbind() override;

    void connected(HANDLE processHandle);

private:
    HANDLE m_processHandle;
    DWORD m_processIdentifier;
    HWND m_processMessageWindow;
};

ProcessConnection::ProcessConnection()
    : m_processHandle(INVALID_HANDLE_VALUE)
    , m_processIdentifier(0)
    , m_processMessageWindow(NULL)
{
}

ProcessConnection::~ProcessConnection()
{
}

void ProcessConnection::unbind()
{
    ::TerminateProcess(m_processHandle, 0);
}

void ProcessConnection::connected(HANDLE processHandle)
{
    m_processHandle = processHandle;
    m_processIdentifier = ::GetProcessId(processHandle);
}

static const wchar_t* processExecutableName()
{
    static const wchar_t name[] = L"sling.exe";
    return name;
}

void ProcessLauncher::platformInitialize()
{
}

ProcessLauncher::Connection* ProcessLauncher::platformCreateProcess(const String& moduleName, const String& moduleEntry, const String& arguments, const std::vector<int32_t>& fileDescriptors)
{
    std::vector<HANDLE> fileDescriptorHandles;
    String fileDescriptorHandleMap;

    for (auto fileDescriptor : fileDescriptors) {
        HANDLE fileDescriptorHandle = reinterpret_cast<HANDLE>(win32_release_osfhandle(fileDescriptor));
        // Ensure that the child process inherits the file descriptor handles.
        ::SetHandleInformation(fileDescriptorHandle, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
        fileDescriptorHandles.push_back(fileDescriptorHandle);
        fileDescriptorHandleMap.append(std::to_wstring(fileDescriptor));
        fileDescriptorHandleMap.append(L":");
        fileDescriptorHandleMap.append(std::to_wstring((intptr_t)fileDescriptorHandle));
        fileDescriptorHandleMap.append(L";");
    }

    STARTUPINFOW startupInfo = { 0 };
    startupInfo.cb = sizeof(startupInfo);

    PROCESS_INFORMATION processInformation = { 0 };

    // Start the target process suspended.
    DWORD flags = CREATE_SUSPENDED;
    if (!IsWindows8OrGreater()) {
        // Windows 8 implements nested jobs, but for older systems we need to
        // break out of any job we're in to enforce our restrictions.
        flags |= CREATE_BREAKAWAY_FROM_JOB;
    }

    std::unique_ptr<ProcessConnection> connection(new ProcessConnection);

    String commandLine;
    commandLine.append(L"\"");
    commandLine.append(System::getSystemPath() + L"\\" + processExecutableName());
    commandLine.append(L"\"");
    commandLine.append(L" \"");
    commandLine.append(System::getModulePath(moduleName));
    commandLine.append(L"\"");
    commandLine.append(L" \"");
    commandLine.append(moduleEntry);
    commandLine.append(L"\"");
    commandLine.append(L" \"");
    commandLine.append(arguments);
    commandLine.append(L"\"");
    commandLine.append(L" \"");
    commandLine.append(fileDescriptorHandleMap);
    commandLine.append(L"\"");
    commandLine.append(L" \"");
    commandLine.append(std::to_wstring(connection->connectionIdentifier()));
    commandLine.append(L"\"");
    commandLine.append(L" \"");
    commandLine.append(std::to_wstring((intptr_t)m_messageReceiver->getBinder()));
    commandLine.append(L"\"");

    BOOL result = ::CreateProcessW(0, const_cast<wchar_t*>(commandLine.c_str()),
        0, 0, true, flags,
        0, 0, &startupInfo, &processInformation);

    for (auto fileDescriptorHandle : fileDescriptorHandles) {
        // We can now close the file descriptor handles.
        ::CloseHandle(fileDescriptorHandle);
    }

    if (!result) {
        // FIXME: What should we do here?
        DWORD error = ::GetLastError();
        assert(false);
        return nullptr;
    }

    if (!IsWindows8OrGreater()) {
        BOOL inJob = FALSE;
        IsProcessInJob(processInformation.hProcess, NULL, &inJob);
        if (inJob) {
            assert(false);
            return nullptr;
        }
    }

    connection->connected(processInformation.hProcess);

    ::AssignProcessToJobObject(JobObject::handle(), processInformation.hProcess);
    ::ResumeThread(processInformation.hThread);

    // Don't leak the thread handle.
    ::CloseHandle(processInformation.hThread);

    return connection.release();
}

} // namespace appkit
} // namespace os
} // namespace android
