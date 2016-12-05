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

#include <crtdbg.h>
#include <shlwapi.h>
#include <windows.h>

#if defined _M_IX86
#define PROCESSORARCHITECTURE "x86"
#elif defined _M_IA64
#define PROCESSORARCHITECTURE "ia64"
#elif defined _M_X64
#define PROCESSORARCHITECTURE "amd64"
#else
#define PROCESSORARCHITECTURE "*"
#endif

#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='" PROCESSORARCHITECTURE "' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <android/os/Looper.h>
#include <android/os/appkit/Process.h>
#include <platforms/StringConversion.h>

#include <sys/socket.h>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpstrCmdLine, int nCmdShow)
{
    std::vector<String> options = std::split(std::wstring(lpstrCmdLine), L'\"');
    std::vector<String> fileDescriptorsKeyValue = std::split(options[3], L';');

    std::vector<int32_t> fileDescriptors;
    for (auto& fileDescriptorKeyValue : fileDescriptorsKeyValue) {
        std::vector<String> keyValueStrings = std::split(fileDescriptorKeyValue, L':');
        std::pair<int32_t, uint64_t> keyValue = std::make_pair(std::wcstol(keyValueStrings[0].c_str(), NULL, 10), std::wcstoll(keyValueStrings[1].c_str(), NULL, 10));
        fileDescriptors.push_back(keyValue.first);
        win32_open_osfhandle(keyValue.second, 0, keyValue.first);
    }

    std::unordered_map<String, String> platformMainParameters;
    platformMainParameters[L"hInstance"] = std::to_wstring((uint64_t)hInstance);
    platformMainParameters[L"hPrevInstance"] = std::to_wstring((uint64_t)hPrevInstance);
    platformMainParameters[L"nCmdShow"] = std::to_wstring(nCmdShow);

    std::unique_ptr<Process> process = Process::create(options[0], options[1], options[2], fileDescriptors, std::wcstol(options[4].c_str(), NULL, 10), std::wcstoll(options[5].c_str(), NULL, 10), platformMainParameters);
    if (!process)
        return -1;

    return process->start();
}
