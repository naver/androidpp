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
#include <android/opengl/appkit/GLTextureHandle.h>

namespace android {
namespace opengl {
namespace appkit {

class EGLPbufferSurface;

class EGLOffscreenContext final {
public:
    ANDROID_EXPORT static std::shared_ptr<EGLOffscreenContext> create(EGLPbufferSurface&, EGLContext);
    ANDROID_EXPORT virtual ~EGLOffscreenContext();

    ANDROID_EXPORT bool makeCurrent();
    ANDROID_EXPORT bool doneCurrent();

    EGLContext eglContext() { return m_eglContext; }
    EGLPbufferSurface& frontBufferSurface() { return m_offscreenSurface; }

    ANDROID_EXPORT void copyFromTexture(GLTextureHandle&, bool);

    ANDROID_EXPORT bool isContextLost() { return m_contextLost; }

private:
    EGLOffscreenContext(EGLPbufferSurface&, EGLContext);

    void initialize(EGLContext);
    void initializeShaders();
    void copyFromTextureGL(GLuint texture, bool flip);

    EGLContext m_detachedContext;
    EGLSurface m_detachedReadSurface;
    EGLSurface m_detachedDrawSurface;
    EGLDisplay m_eglDisplay;
    EGLContext m_eglContext;
    EGLConfig m_eglConfig;
    int32_t m_madeCurrent;
    GLuint m_shaderProgram;
    GLuint m_vertexAttr;
    GLuint m_textureCoordinatesAttr;
    GLuint m_textureUniform;
    EGLPbufferSurface& m_offscreenSurface;
    bool m_supportsRobustnessExtension;
    bool m_contextLost;
};

} // namespace appkit
} // namespace opengl
} // namespace android

using EGLOffscreenContext = android::opengl::appkit::EGLOffscreenContext;
