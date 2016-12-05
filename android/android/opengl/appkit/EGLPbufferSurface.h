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
#include <android/opengl/appkit/GLTransportSurface.h>

namespace android {
namespace opengl {
namespace appkit {

class EGLOffscreenContext;

class EGLPbufferSurface : public GLTransportSurface {
public:
    ANDROID_EXPORT static std::shared_ptr<EGLPbufferSurface> create(EGLDisplay dpy, EGLConfig config);
    ANDROID_EXPORT static std::shared_ptr<EGLPbufferSurface> create(EGLDisplay dpy, EGLConfig config, EGLContext shareContext, EGLint width, EGLint height);
    ANDROID_EXPORT static std::shared_ptr<EGLPbufferSurface> import(EGLConfig config, int64_t shareHandle, EGLint width, EGLint height);
    ANDROID_EXPORT virtual ~EGLPbufferSurface();

    EGLSurface getNativeHandle() { return m_eglSurface; }

    EGLDisplay eglDisplay() { return m_eglDisplay; }
    EGLConfig eglConfig() { return m_eglConfig; }
    EGLOffscreenContext* getContext() { return m_offscreenContext.get(); }

    ANDROID_EXPORT void swapBuffers() override;
    ANDROID_EXPORT bool copyFromTexture(GLTextureHandle&, bool) override;
    ANDROID_EXPORT GLuint getGLTexture() override;

    ANDROID_EXPORT void reset() override;
    ANDROID_EXPORT void reset(int64_t shareHandle, EGLint width, EGLint height) override;
    ANDROID_EXPORT void reset(EGLContext shareContext, EGLint width, EGLint height) override;
    ANDROID_EXPORT void publish() override;

    ANDROID_EXPORT bool isValid() override;

protected:
    EGLPbufferSurface(EGLDisplay, EGLConfig);
    EGLPbufferSurface(EGLDisplay, EGLConfig, EGLContext, EGLint, EGLint);
    EGLPbufferSurface(EGLConfig, int64_t, EGLint, EGLint);

    void bindSurface();

    bool platformCreate(EGLContext shareContext);
    bool platformImport();

    EGLDisplay m_eglDisplay;
    EGLSurface m_eglSurface;
    EGLConfig m_eglConfig;
    EGLint m_width;
    EGLint m_height;
    intptr_t m_shareHandle;
    bool m_published;
    bool m_surfaceChanged;
    GLuint m_frontBufferTexture;
    std::shared_ptr<EGLOffscreenContext> m_offscreenContext;
};

} // namespace appkit
} // namespace opengl
} // namespace android

using EGLPbufferSurface = android::opengl::appkit::EGLPbufferSurface;
