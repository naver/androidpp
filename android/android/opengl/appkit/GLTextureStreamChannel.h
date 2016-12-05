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

#include <android/opengl/appkit/GLTransportSurface.h>
#include <android/os/appkit/MessageChannel.h>

namespace android {
namespace opengl {
namespace appkit {

class GLTextureStreamChannel : public MessageChannel {
public:
    ANDROID_EXPORT static std::unique_ptr<GLTextureStreamChannel> create(GLTransportSurface& surface);
    ANDROID_EXPORT virtual ~GLTextureStreamChannel();

    GLTransportSurface& surface() { return m_surface; }

    int32_t shareIdentifier() { return channelIdentifier(); }

    ANDROID_EXPORT void connect();
    ANDROID_EXPORT void reshape(EGLint width, EGLint height);
    ANDROID_EXPORT void publish(GLTransportSurface::Type, intptr_t shareHandle);

protected:
    ANDROID_EXPORT GLTextureStreamChannel(GLTransportSurface&);

    // os::MessageChannel
    ANDROID_EXPORT void receive(const os::Message&) override;

private:
    GLTransportSurface& m_surface;
};

} // namespace appkit
} // namespace opengl
} // namespace android

using GLTextureStreamChannel = android::opengl::appkit::GLTextureStreamChannel;
