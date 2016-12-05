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

#include "BundlePrivate.h"

#include <platforms/LogHelper.h>
#include <platforms/StringConversion.h>

#include <json/json.h>

#include <streambuf>

namespace android {
namespace os {

class BundlePrivateJSON final : public BundlePrivate {
public:
    BundlePrivateJSON() = default;
    ~BundlePrivateJSON() = default;

    CharSequence getCharSequence(const String& key) override;
    void putCharSequence(const String& key, const CharSequence& value) override;

    void writeToParcel(Parcel& dest, int32_t flags) override;
    void readFromParcel(const Parcel& parcel) override;

private:
    Json::Value m_root;
};

std::shared_ptr<BundlePrivate> BundlePrivate::create()
{
    return std::shared_ptr<BundlePrivate>(new BundlePrivateJSON);
}

CharSequence BundlePrivateJSON::getCharSequence(const String& key)
{
    return std::s2ws(m_root[std::ws2s(key)].asString());
}

void BundlePrivateJSON::putCharSequence(const String& key, const CharSequence& value)
{
    m_root[std::ws2s(key)] = std::ws2s(value);
}

void BundlePrivateJSON::writeToParcel(Parcel& dest, int32_t flags)
{
    Json::StreamWriterBuilder builder;
    std::string document = Json::writeString(builder, m_root);
    dest.insert(dest.end(), document.data(), document.data() + document.size());
}

struct vectorstreambuf : public std::streambuf {
    vectorstreambuf(const std::vector<char>& v)
    {
        setg(const_cast<char*>(v.data()), const_cast<char*>(v.data()), const_cast<char*>(v.data()) + v.size());
    }
};

void BundlePrivateJSON::readFromParcel(const Parcel& parcel)
{
    Json::CharReaderBuilder builder;
    vectorstreambuf streamBuffer(parcel);
    std::istream stream(&streamBuffer);
    std::string errors;
    bool ok = Json::parseFromStream(builder, stream, &m_root, &errors);
    if (!ok) {
        LOGE("Error at BundlePrivateJSON::readFromParcel() - %s", errors.c_str());
    }
}

} // namespace os
} // namespace android
