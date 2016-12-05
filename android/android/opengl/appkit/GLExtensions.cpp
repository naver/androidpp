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

#include "GLExtensions.h"

#include <platforms/StringConversion.h>

#include <GLES2/gl2.h>

#include <mutex>
#include <unordered_set>

namespace android {
namespace opengl {
namespace appkit {

static std::unordered_set<std::string> supportedExtensions;

bool GLExtensions::supportsExtension(const String& extension)
{
    static std::once_flag onceFlag;
    std::call_once(onceFlag, [] {
        std::vector<std::string> extensions = std::split(std::string(reinterpret_cast<const char*>(::glGetString(GL_EXTENSIONS))), ' ');
        for (size_t i = 0; i < extensions.size(); ++i)
            supportedExtensions.insert(extensions[i]);
    });
    return supportedExtensions.count(std::ws2s(extension)) > 0;
}

} // namespace appkit
} // namespace opengl
} // namespace android
