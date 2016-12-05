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

#include "Bundle.h"

#include <android/os/BundlePrivate.h>
#include <assert>

namespace android {
namespace os {

Bundle::Bundle()
    : m_private(BundlePrivate::create())
{
}

Bundle::Bundle(const Bundle& o)
    : m_private(o.m_private)
{
}

Bundle::Bundle(Bundle&& o)
    : m_private(std::move(o.m_private))
{
}

CharSequence Bundle::getCharSequence(const String& key, const CharSequence& defaultValue)
{
    CharSequence result = getCharSequence(key);
    if (result.empty())
        return defaultValue;

    return result;
}

CharSequence Bundle::getCharSequence(const String& key)
{
    return m_private->getCharSequence(key);
}

void Bundle::putCharSequence(const String& key, const CharSequence& value)
{
    m_private->putCharSequence(key, value);
}

void Bundle::writeToParcel(Parcel& dest, int32_t flags)
{
    m_private->writeToParcel(dest, flags);
}

void Bundle::readFromParcel(Parcel& parcel)
{
    m_private->readFromParcel(parcel);
}

} // namespace os
} // namespace android
