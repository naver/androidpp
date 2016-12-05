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
namespace opengl {
namespace appkit {

struct GLTextureStreamHostMessages : public Messages {
    const int32_t CONNECT;
    const int32_t RESHAPE;
    const int32_t PUBLISH;

    static GLTextureStreamHostMessages& get()
    {
        static GLTextureStreamHostMessages shared;
        return shared;
    }

    inline Message Reshape(int32_t width, int32_t height)
    {
        return Message::obtain(0, RESHAPE, width, height);
    }

    inline Message Publish(int32_t surfaceType, intptr_t shareHandle)
    {
        return Message::obtain(0, PUBLISH, surfaceType, 0, shareHandle);
    }

private:
    GLTextureStreamHostMessages()
        : CONNECT(getUniqueMessageIdentifier(0))
        , RESHAPE(getUniqueMessageIdentifier(1))
        , PUBLISH(getUniqueMessageIdentifier(2))
    {
    }
};

} // namespace appkit
} // namespace opengl
} // namespace android

using GLTextureStreamHostMessages = android::opengl::appkit::GLTextureStreamHostMessages;
