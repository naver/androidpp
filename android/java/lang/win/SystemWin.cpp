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

#include "System.h"

#include <platforms/StringConversion.h>

#include <assert>
#include <shlwapi.h>

namespace java {
namespace lang {

static const wchar_t* systemDLLName()
{
    static const wchar_t name[] = L"os.dll";
    return name;
}

static String modulePath(const wchar_t* moduleName)
{
    HMODULE module = ::GetModuleHandleW(moduleName);
    if (!module) {
        module = ::GetModuleHandleW(NULL);
        if (!module) {
            DWORD error = ::GetLastError();
            assert(false);
            return L"";
        }
    }

    WCHAR modulePath[MAX_PATH];
    if (!::GetModuleFileNameW(module, modulePath, sizeof(modulePath) / sizeof(modulePath[0]))) {
        DWORD error = ::GetLastError();
        assert(false);
        return L"";
    }

    ::PathRemoveFileSpecW(modulePath);
    return modulePath;
}

String System::getSystemPath()
{
    return modulePath(systemDLLName());
}

String System::getModulePath(const String& moduleName)
{
    return modulePath(moduleName.c_str()) + L"\\" + moduleName + L".dll";
}

String System::currentProcessPath()
{
    return modulePath(NULL);
}

void System::loadLibrary(String& libName)
{
    HMODULE libraryModule = ::LoadLibraryW(libName.c_str());
    if (!libraryModule) {
        DWORD error = ::GetLastError();
        assert(false);
    }
}

} // namespace lang
} // namespace java
