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

#include <android/os/Parcel.h>

namespace android {
namespace os {

class BundlePrivate;

class Bundle {
    friend class BundlePrivate;
public:
    ANDROID_EXPORT Bundle();
    ANDROID_EXPORT Bundle(const Bundle&);
    ANDROID_EXPORT Bundle(Bundle&&);
    ANDROID_EXPORT virtual ~Bundle() = default;

    ANDROID_EXPORT CharSequence getCharSequence(const String& key, const CharSequence& defaultValue);
    ANDROID_EXPORT CharSequence getCharSequence(const String& key);
    ANDROID_EXPORT void putCharSequence(const String& key, const CharSequence& value);

    ANDROID_EXPORT void writeToParcel(Parcel& dest, int32_t flags);
    ANDROID_EXPORT void readFromParcel(Parcel& parcel);

private:
    std::shared_ptr<BundlePrivate> m_private;
};

} // namespace os
} // namespace android

using Bundle = android::os::Bundle;
