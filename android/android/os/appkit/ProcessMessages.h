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

#include <android/os/appkit/Messages.h>

namespace android {
namespace os {
namespace appkit {

class ProcessMessages : public Messages {
public:
    const int32_t PROCESS_LAUNCHED;
    const int32_t LOAD_LIBRARY;

    static ProcessMessages& get()
    {
        static ProcessMessages shared;
        return shared;
    }

    static const String& libraryNameKey()
    {
        static const String name(L"libraryName");
        return name;
    }

    inline Message ProcessLaunched(std::shared_ptr<Messenger>& replyTo, int32_t connectionIdentifier)
    {
        return obtain(replyTo, PROCESS_LAUNCHED, connectionIdentifier);
    }

    inline Message LoadLibrary(const String& libraryName)
    {
        Message message = Message::obtain(nullptr, LOAD_LIBRARY);
        message.getData().putCharSequence(libraryNameKey(), libraryName);
        return message;
    }

private:
    ProcessMessages()
        : PROCESS_LAUNCHED(getUniqueMessageIdentifier(0))
        , LOAD_LIBRARY(getUniqueMessageIdentifier(1))
    {
    }
};

} // namespace appkit
} // namespace os
} // namespace android

using ProcessMessages = android::os::appkit::ProcessMessages;
