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

#include <android/os/appkit/Process.h>

#include <android/os/Handler.h>
#include <android/os/Looper.h>
#include <android/os/Messenger.h>
#include <java/lang/System.h>
#include <platforms/StringConversion.h>

#include <shlwapi.h>

#include <assert>

namespace android {
namespace os {
namespace appkit {

typedef int (__cdecl* MainProcPtr)(int, LPTSTR[]);

static const wchar_t instanceHandleKey[] = L"hInstance";
static const wchar_t prevInstanceHandleKey[] = L"hPrevInstance";
static const wchar_t cmdShowKey[] = L"nCmdShow";

// Code from Webkit (https://webkit.org/) under LGPL v2 and BSD licenses (https://webkit.org/licensing-webkit/)
static void enableDataExecutionPrevention()
{
    // Enable Data Execution prevention at runtime rather than via /NXCOMPAT
    // http://blogs.msdn.com/michael_howard/archive/2008/01/29/new-nx-apis-added-to-windows-vista-sp1-windows-xp-sp3-and-windows-server-2008.aspx

    const DWORD enableDEP = 0x00000001;

    HMODULE hMod = ::GetModuleHandleW(L"Kernel32.dll");
    if (!hMod)
        return;

    typedef BOOL (WINAPI *PSETDEP)(DWORD);

    PSETDEP procSet = reinterpret_cast<PSETDEP>(::GetProcAddress(hMod, "SetProcessDEPPolicy"));
    if (!procSet)
        return;

    // Enable Data Execution Prevention, but allow ATL thunks (for compatibility with the version of ATL that ships with the Platform SDK).
    procSet(enableDEP);
}

static void enableTerminationOnHeapCorruption()
{
    // Enable termination on heap corruption on OSes that support it (Vista and XPSP3).
    // http://msdn.microsoft.com/en-us/library/aa366705(VS.85).aspx

    const HEAP_INFORMATION_CLASS heapEnableTerminationOnCorruption = static_cast<HEAP_INFORMATION_CLASS>(1);

    HMODULE hMod = ::GetModuleHandleW(L"kernel32.dll");
    if (!hMod)
        return;

    typedef BOOL (WINAPI*HSI)(HANDLE, HEAP_INFORMATION_CLASS, PVOID, SIZE_T);
    HSI heapSetInformation = reinterpret_cast<HSI>(::GetProcAddress(hMod, "HeapSetInformation"));
    if (!heapSetInformation)
        return;

    heapSetInformation(0, heapEnableTerminationOnCorruption, 0, 0);
}

static void disableUserModeCallbackExceptionFilter()
{
    const DWORD PROCESS_CALLBACK_FILTER_ENABLED = 0x1;
    typedef BOOL (NTAPI *getProcessUserModeExceptionPolicyPtr)(LPDWORD lpFlags);
    typedef BOOL (NTAPI *setProcessUserModeExceptionPolicyPtr)(DWORD dwFlags);

    HMODULE lib = LoadLibrary(TEXT("kernel32.dll"));
    assert(lib);

    getProcessUserModeExceptionPolicyPtr getPolicyPtr = (getProcessUserModeExceptionPolicyPtr)GetProcAddress(lib, "GetProcessUserModeExceptionPolicy");
    setProcessUserModeExceptionPolicyPtr setPolicyPtr = (setProcessUserModeExceptionPolicyPtr)GetProcAddress(lib, "SetProcessUserModeExceptionPolicy");

    DWORD dwFlags;
    if (!getPolicyPtr || !setPolicyPtr || !getPolicyPtr(&dwFlags)) {
        FreeLibrary(lib);
        return;
    }

    // If this flag isn't cleared, exceptions that are thrown when running in a 64-bit version of
    // Windows are ignored, possibly leaving Safari in an inconsistent state that could cause an 
    // unrelated exception to be thrown.
    // http://support.microsoft.com/kb/976038
    // http://blog.paulbetts.org/index.php/2010/07/20/the-case-of-the-disappearing-onload-exception-user-mode-callback-exceptions-in-x64/
    setPolicyPtr(dwFlags & ~PROCESS_CALLBACK_FILTER_ENABLED);

    FreeLibrary(lib);
}

#ifndef NDEBUG
static void pauseProcessIfNeeded(HMODULE module)
{
    // Show an alert when Ctrl-Alt-Shift is held down during launch to give the user time to attach a
    // debugger. This is useful for debugging problems that happen early in the web process's lifetime.
    const unsigned short highBitMaskShort = 0x8000;
    if (!getenv("SLING_PAUSE_PROCESS_ON_LAUNCH") && !((::GetKeyState(VK_CONTROL) & highBitMaskShort) && (::GetKeyState(VK_MENU) & highBitMaskShort) && (::GetKeyState(VK_SHIFT) & highBitMaskShort)))
        return;

    wchar_t path[MAX_PATH];
    DWORD length = ::GetModuleFileNameW(module, path, sizeof(path));
    if (!length || length == sizeof(path))
        return;

    wchar_t* startOfFilename = ::PathFindFileNameW(path);
    String filenameString(startOfFilename, length - (startOfFilename - path));

    String message = L"You can now attach a debugger to " + filenameString + L". You can use the same debugger for " + filenameString + L" and the UI process, if desired. Click OK when you are ready for " + filenameString + L" to continue.";
    String title = filenameString + L" has launched";
    ::MessageBoxW(0, message.c_str(), title.c_str(), MB_OK | MB_ICONINFORMATION);
}
#endif

class ProcessWin : public Process {
public:
    ProcessWin(const String& modulePath, const String& moduleEntry, const String& arguments, const std::vector<int32_t>& fileDescriptors,
        int32_t connectionIdentifier, IBinder targetHandle, const std::unordered_map<String, String>& platformMainParameters);
    ~ProcessWin();

private:
    int32_t platformStart() override;

    HINSTANCE instanceHandle() const;
    HINSTANCE prevInstanceHandle() const;
    int cmdShow() const;

    MainProcPtr m_mainProc;
    String m_arguments;
    LPTSTR m_argv;
    std::unordered_map<String, String> m_winMainParameters;
};

ProcessWin::ProcessWin(const String& modulePath, const String& moduleEntry, const String& arguments, const std::vector<int32_t>& fileDescriptors, int32_t connectionIdentifier, IBinder targetHandle, const std::unordered_map<String, String>& platformMainParameters)
    : Process(connectionIdentifier, targetHandle)
    , m_mainProc(NULL)
{
    HMODULE module = ::LoadLibraryW(modulePath.c_str());
    m_mainProc = reinterpret_cast<MainProcPtr>(GetProcAddress(module, std::ws2s(moduleEntry).c_str()));
    if (!m_mainProc) {
        assert(false);
        return;
    }

    m_arguments = arguments;
    m_argv = (LPTSTR)m_arguments.c_str();
    m_winMainParameters = platformMainParameters;

#ifndef NDEBUG
    pauseProcessIfNeeded(instanceHandle());
#endif
    
    enableDataExecutionPrevention();
    enableTerminationOnHeapCorruption();
    disableUserModeCallbackExceptionFilter();

    processLaunched();
}

ProcessWin::~ProcessWin()
{
}

HINSTANCE ProcessWin::instanceHandle() const
{
    assert(m_winMainParameters.count(instanceHandleKey) > 0);
    return reinterpret_cast<HINSTANCE>(std::wcstoll(m_winMainParameters.at(instanceHandleKey).c_str(), NULL, 10));
}

HINSTANCE ProcessWin::prevInstanceHandle() const
{
    assert(m_winMainParameters.count(prevInstanceHandleKey) > 0);
    return reinterpret_cast<HINSTANCE>(std::wcstoll(m_winMainParameters.at(prevInstanceHandleKey).c_str(), NULL, 10));
}

int ProcessWin::cmdShow() const
{
    assert(m_winMainParameters.count(cmdShowKey) > 0);
    return std::wcstol(m_winMainParameters.at(cmdShowKey).c_str(), NULL, 10);
}

Process* Process::platformCreate(const String& modulePath, const String& moduleEntry, const String& arguments, const std::vector<int32_t>& fileDescriptors, int32_t connectionIdentifier, IBinder targetHandle, const std::unordered_map<String, String>& platformMainParameters)
{
    return new ProcessWin(modulePath, moduleEntry, arguments, fileDescriptors, connectionIdentifier, targetHandle, platformMainParameters);
}

int32_t ProcessWin::platformStart()
{
    HRESULT hRes = ::OleInitialize(NULL);
    assert(SUCCEEDED(hRes));

    int32_t result = m_mainProc(1, &m_argv);

    ::OleUninitialize();
    return result;
}

} // namespace appkit
} // namespace os
} // namespace android
