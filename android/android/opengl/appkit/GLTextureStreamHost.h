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
#include <android/os/appkit/MessageHost.h>

namespace android {
namespace opengl {
namespace appkit {

class GLTextureStreamHost : public MessageHost {
public:
    ANDROID_EXPORT virtual ~GLTextureStreamHost();

    ANDROID_EXPORT static void initializeOnce();

    ANDROID_EXPORT std::shared_ptr<GLTransportSurface> import(EGLConfig config);
    ANDROID_EXPORT std::shared_ptr<GLTransportSurface> surface() { return m_surface.lock(); }

protected:
    ANDROID_EXPORT GLTextureStreamHost(std::shared_ptr<Messenger>&&);

    // MessageHost
    ANDROID_EXPORT void receive(Message&) override;

private:
    static void setReceiveMessages(bool& onceFlag, Messages&);
    static bool receiveMessage(Messenger&, Message&);

    int32_t m_width;
    int32_t m_height;
    GLTransportSurface::Type m_surfaceType;
    intptr_t m_shareHandle;
    std::weak_ptr<GLTransportSurface> m_surface;
};

} // namespace appkit
} // namespace opengl
} // namespace android

using GLTextureStreamHost = android::opengl::appkit::GLTextureStreamHost;
