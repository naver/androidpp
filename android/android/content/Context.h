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
#include <android/content/res/Resources.h>

namespace android {
namespace content {

class ContextPrivate;

class Context {
    friend class ContextPrivate;
public:
    // Use with getSystemService(Class) to retrieve a InputMethodManager for accessing input methods.
    ANDROID_EXPORT static wchar_t INPUT_METHOD_SERVICE[];

    ANDROID_EXPORT Context();
    ANDROID_EXPORT virtual ~Context();

    // Return the context of the single, global Application object of the current process.
    ANDROID_EXPORT virtual Context& getApplicationContext() = 0;
    // Return the handle to a system-level service by name.
    ANDROID_EXPORT virtual std::shared_ptr<Object> getSystemService(const String& name) = 0;
    // Returns a Resources instance for the application's package.
    ANDROID_EXPORT virtual Resources& getResources() = 0;

private:
    std::unique_ptr<ContextPrivate> m_private;
};

} // namespace content
} // namespace android

using Context = android::content::Context;
