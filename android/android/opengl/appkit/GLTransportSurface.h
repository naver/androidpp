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

#include <android/opengl/EGL.h>

#include <GLES2/gl2.h>

namespace android {
namespace opengl {
namespace appkit {

class GLTextureHandle;
class GLTextureStreamChannel;

class GLTransportSurface {
public:
    enum Type {
        Unknown,
        Token,
        EGLPbuffer,
    };

    class Backbuffer {
    public:
        virtual ~Backbuffer() = default;
        virtual void swapBuffers(GLTransportSurface&) = 0;
    };

    ANDROID_EXPORT static std::shared_ptr<GLTransportSurface> import(EGLConfig, int32_t shareIdentifier);
    ANDROID_EXPORT virtual ~GLTransportSurface();

    ANDROID_EXPORT virtual void setBackbuffer(Backbuffer*);
    ANDROID_EXPORT virtual void swapBuffers();
    ANDROID_EXPORT virtual bool copyFromTexture(GLTextureHandle&, bool) = 0;
    ANDROID_EXPORT virtual GLuint getGLTexture() = 0;

    ANDROID_EXPORT virtual void reset() = 0;
    ANDROID_EXPORT virtual void reset(int64_t shareHandle, EGLint width, EGLint height) = 0;
    ANDROID_EXPORT virtual void reset(EGLContext shareContext, EGLint width, EGLint height) = 0;
    ANDROID_EXPORT virtual void publish() = 0;

    ANDROID_EXPORT virtual bool isValid() = 0;

    GLTextureStreamChannel* channel() { return m_channel.get(); }

protected:
    ANDROID_EXPORT GLTransportSurface();

    ANDROID_EXPORT void connect();
    ANDROID_EXPORT void reshape(EGLint width, EGLint height);
    ANDROID_EXPORT void publish(Type, intptr_t shareHandle);

    Backbuffer* backbuffer() { return m_backbuffer; }

private:
    Type m_surfaceType;
    Backbuffer* m_backbuffer;
    std::unique_ptr<GLTextureStreamChannel> m_channel;
};

} // namespace appkit
} // namespace opengl
} // namespace android

using GLTransportSurface = android::opengl::appkit::GLTransportSurface;
