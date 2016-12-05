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

#include "EGLPbufferSurface.h"

#include "EGLOffscreenContext.h"

#include <assert>

namespace android {
namespace opengl {
namespace appkit {

std::shared_ptr<EGLPbufferSurface> EGLPbufferSurface::create(EGLDisplay dpy, EGLConfig config)
{
    return std::shared_ptr<EGLPbufferSurface>(new EGLPbufferSurface(dpy, config));
}

std::shared_ptr<EGLPbufferSurface> EGLPbufferSurface::create(EGLDisplay dpy, EGLConfig config, EGLContext shareContext, EGLint width, EGLint height)
{
    return std::shared_ptr<EGLPbufferSurface>(new EGLPbufferSurface(dpy, config, shareContext, width, height));
}

std::shared_ptr<EGLPbufferSurface> EGLPbufferSurface::import(EGLConfig config, int64_t shareHandle, EGLint width, EGLint height)
{
    return std::shared_ptr<EGLPbufferSurface>(new EGLPbufferSurface(config, shareHandle, width, height));
}

static EGLDisplay eglGetSharedDisplay()
{
    EGLDisplay sharedDisplay = ::eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (sharedDisplay != EGL_NO_DISPLAY && (!eglInitialize(sharedDisplay, 0, 0) || !eglBindAPI(EGL_OPENGL_ES_API)))
        sharedDisplay = EGL_NO_DISPLAY;
    return sharedDisplay;
}

static EGLConfig eglGetConfig(EGLDisplay dpy, EGLConfig* config)
{
    EGLint attributeList[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_STENCIL_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_NONE
    };

    EGLint numberConfigsReturned;
    bool ok = ::eglChooseConfig(dpy, attributeList, config, 1, &numberConfigsReturned) && numberConfigsReturned;
    return *config;
}

EGLPbufferSurface::EGLPbufferSurface(EGLDisplay dpy, EGLConfig config)
    : m_eglDisplay((dpy) ? dpy : eglGetSharedDisplay())
    , m_eglSurface(EGL_NO_SURFACE)
    , m_eglConfig((config) ? config : eglGetConfig(m_eglDisplay, &config))
    , m_width(0)
    , m_height(0)
    , m_shareHandle(0)
    , m_published(false)
    , m_surfaceChanged(false)
    , m_frontBufferTexture(0)
{
    GLTransportSurface::connect();
}

EGLPbufferSurface::EGLPbufferSurface(EGLDisplay dpy, EGLConfig config, EGLContext shareContext, EGLint width, EGLint height)
    : m_eglDisplay((dpy) ? dpy : eglGetSharedDisplay())
    , m_eglSurface(EGL_NO_SURFACE)
    , m_eglConfig((config) ? config : eglGetConfig(m_eglDisplay, &config))
    , m_width(width)
    , m_height(height)
    , m_shareHandle(0)
    , m_published(false)
    , m_surfaceChanged(false)
    , m_frontBufferTexture(0)
{
    GLTransportSurface::connect();
    reset(shareContext, width, height);
}

EGLPbufferSurface::EGLPbufferSurface(EGLConfig config, int64_t shareHandle, EGLint width, EGLint height)
    : m_eglDisplay(::eglGetCurrentDisplay())
    , m_eglSurface(EGL_NO_SURFACE)
    , m_eglConfig((config) ? config : eglGetConfig(m_eglDisplay, &config))
    , m_width(width)
    , m_height(height)
    , m_shareHandle(shareHandle)
    , m_published(false)
    , m_surfaceChanged(true)
    , m_frontBufferTexture(0)
{
}

EGLPbufferSurface::~EGLPbufferSurface()
{
    reset();
}

void EGLPbufferSurface::swapBuffers()
{
    bindSurface();

    if (!m_offscreenContext)
        return;

    if (m_offscreenContext->makeCurrent()) {
        GLTransportSurface::swapBuffers();
        m_offscreenContext->doneCurrent();
    }
}

bool EGLPbufferSurface::copyFromTexture(GLTextureHandle& texture, bool flip)
{
    bindSurface();

    if (!m_offscreenContext)
        return false;

    if (m_offscreenContext->makeCurrent()) {
        m_offscreenContext->copyFromTexture(texture, flip);
        m_offscreenContext->doneCurrent();
        return true;
    }

    return false;
}

GLuint EGLPbufferSurface::getGLTexture()
{
    bindSurface();

    return m_frontBufferTexture;
}

void EGLPbufferSurface::reset()
{
    m_offscreenContext.reset();

    if (m_frontBufferTexture) {
        ::eglReleaseTexImage(m_eglDisplay, m_eglSurface, EGL_BACK_BUFFER);
        ::glDeleteTextures(1, &m_frontBufferTexture);
        m_frontBufferTexture = 0;
    }

    if (m_eglSurface) {
        ::eglDestroySurface(m_eglDisplay, m_eglSurface);
        m_eglSurface = EGL_NO_SURFACE;
        m_published = false;
    }

    m_width = 0;
    m_height = 0;
    m_shareHandle = 0;
}

void EGLPbufferSurface::reset(int64_t shareHandle, EGLint width, EGLint height)
{
    m_width = width;
    m_height = height;
    m_shareHandle = shareHandle;
    m_surfaceChanged = true;
}

void EGLPbufferSurface::reset(EGLContext shareContext, EGLint width, EGLint height)
{
    if (m_offscreenContext && m_offscreenContext->eglContext() == shareContext && m_width == width && m_height == height)
        return;

    m_offscreenContext.reset();

    m_width = width;
    m_height = height;

    bool ok = platformCreate(shareContext);
    assert(ok);

    if (m_published)
        m_published = false;

    GLTransportSurface::reshape(width, height);

    m_offscreenContext = EGLOffscreenContext::create(*this, shareContext);
}

void EGLPbufferSurface::publish()
{
    if (m_published)
        return;

    m_published = true;

    GLTransportSurface::publish(EGLPbuffer, m_shareHandle);
}

bool EGLPbufferSurface::isValid()
{
    return !m_offscreenContext->isContextLost();
}

void EGLPbufferSurface::bindSurface()
{
    if (!m_surfaceChanged)
        return;

    m_surfaceChanged = false;

    if (!m_frontBufferTexture)
        glGenTextures(1, &m_frontBufferTexture);
    else if (m_eglSurface) {
        ::eglReleaseTexImage(m_eglDisplay, m_eglSurface, EGL_BACK_BUFFER);
        ::eglDestroySurface(m_eglDisplay, m_eglSurface);
        m_eglSurface = EGL_NO_SURFACE;
    }

    assert(m_eglSurface == EGL_NO_SURFACE);

    bool ok = platformImport();
    assert(ok);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_frontBufferTexture);
    eglBindTexImage(m_eglDisplay, m_eglSurface, EGL_BACK_BUFFER);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

} // namespace appkit
} // namespace opengl
} // namespace android
