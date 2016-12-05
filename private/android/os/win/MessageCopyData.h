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

#include <android/os/Message.h>

#include <array>
#include <assert.h>

namespace android {
namespace os {

struct MessageCopyData {
    static const size_t sizeLimit = 1024; // 4K

    enum DataField {
        MessageWhat,
        MessageArg1,
        MessageArg2,
        MessageObj,
        MessageBundleSize,
        MessageBundleData,
    };

    typedef std::array<intptr_t, sizeLimit> Data;

    static size_t pack(Data&, Message&);
    static bool unpack(Message&, const Data&);
};

inline size_t MessageCopyData::pack(Data& out, Message& in)
{
    out[MessageWhat] = in.what;
    out[MessageArg1] = in.arg1;
    out[MessageArg2] = in.arg2;
    out[MessageObj] = in.obj;

    intptr_t messageBundleSize = 0;
    Parcel parcel;
    if (in.peekData()) {
        in.peekData()->writeToParcel(parcel, 0);
        assert(parcel.size() < (sizeLimit - MessageBundleData) * sizeof(intptr_t));
        messageBundleSize = parcel.size();
        if (messageBundleSize > 0)
            memcpy(&out[MessageBundleData], parcel.data(), messageBundleSize);
    }

    out[MessageBundleSize] = messageBundleSize;
    return sizeof(intptr_t) * MessageBundleData + messageBundleSize;
}

inline bool MessageCopyData::unpack(Message& out, const Data& in)
{
    out = Message::obtain(nullptr, in[MessageWhat], in[MessageArg1], in[MessageArg2], in[MessageObj]);

    if (in[MessageBundleSize] > 0) {
        Parcel parcel;
        const char* dataBegin = reinterpret_cast<const char*>(&in[MessageBundleData]);
        const char* dataEnd = dataBegin + in[MessageBundleSize];
        parcel.insert(parcel.end(), dataBegin, dataEnd);
        out.getData().readFromParcel(parcel);
    }

    return true;
}

} // namespace os
} // namespace android
