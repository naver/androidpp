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

#include "EGLOffscreenContext.h"

#include "EGLPbufferSurface.h"
#include "GLExtensions.h"

#include <EGL/eglext.h>
#include <GLES2/gl2ext.h>

#include <platforms/CompilerMacros.h>

#include <assert>

namespace android {
namespace opengl {
namespace appkit {

std::shared_ptr<EGLOffscreenContext> EGLOffscreenContext::create(EGLPbufferSurface& surface, EGLContext shareContext)
{
    return std::shared_ptr<EGLOffscreenContext>(new EGLOffscreenContext(surface, shareContext));
}

EGLOffscreenContext::EGLOffscreenContext(EGLPbufferSurface& surface, EGLContext shareContext)
    : m_detachedContext(EGL_NO_CONTEXT)
    , m_detachedReadSurface(EGL_NO_SURFACE)
    , m_detachedDrawSurface(EGL_NO_SURFACE)
    , m_eglDisplay(surface.eglDisplay())
    , m_eglContext(EGL_NO_CONTEXT)
    , m_eglConfig(surface.eglConfig())
    , m_madeCurrent(0)
    , m_shaderProgram(0)
    , m_vertexAttr(0)
    , m_textureCoordinatesAttr(0)
    , m_textureUniform(0)
    , m_offscreenSurface(surface)
    , m_supportsRobustnessExtension(false)
    , m_contextLost(false)
{
    initialize(shareContext);
}

EGLOffscreenContext::~EGLOffscreenContext()
{
    if (m_shaderProgram) {
        glDeleteProgram(m_shaderProgram);
        m_shaderProgram = 0;
    }

    if (!makeCurrent())
        return;

    m_offscreenSurface.reset();

    if (m_eglDisplay && m_eglContext)
        ::eglDestroyContext(m_eglDisplay, m_eglContext);

    doneCurrent();
}

static bool testEGLContextLost()
{
    static PFNGLGETGRAPHICSRESETSTATUSEXTPROC glGetGraphicsResetStatus = reinterpret_cast<PFNGLGETGRAPHICSRESETSTATUSEXTPROC>(eglGetProcAddress("glGetGraphicsResetStatusEXT"));
    if (!glGetGraphicsResetStatus)
        return false;

    bool contextLost = false;
    switch (glGetGraphicsResetStatus()) {
    case GL_NO_ERROR:
        break;
    case GL_GUILTY_CONTEXT_RESET_EXT:
        contextLost = true;
        break;
    case GL_INNOCENT_CONTEXT_RESET_EXT:
        break;
    case GL_UNKNOWN_CONTEXT_RESET_EXT:
        contextLost = true;
        break;
    default:
        break;
    }
    return contextLost;
}

static THREAD_LOCAL EGLOffscreenContext* currentContext = nullptr;

bool EGLOffscreenContext::makeCurrent()
{
    assert(!currentContext || currentContext == this);

    if (++m_madeCurrent > 1)
        return true;

    currentContext = this;

    m_detachedContext = ::eglGetCurrentContext();
    m_detachedReadSurface = ::eglGetCurrentSurface(EGL_READ);
    m_detachedDrawSurface = ::eglGetCurrentSurface(EGL_DRAW);
    // If any of the values were none, assume they are all none. Not all
    // drivers seem well behaved when it comes to using these values across
    // multiple threads.
    if (m_detachedContext == EGL_NO_CONTEXT || m_detachedReadSurface == EGL_NO_SURFACE || m_detachedDrawSurface == EGL_NO_SURFACE) {
        m_detachedContext = EGL_NO_CONTEXT;
        m_detachedReadSurface = EGL_NO_SURFACE;
        m_detachedDrawSurface = EGL_NO_SURFACE;
    }

    EGLBoolean result = ::eglMakeCurrent(m_eglDisplay, m_offscreenSurface.getNativeHandle(), m_offscreenSurface.getNativeHandle(), m_eglContext);    
    if (result != EGL_TRUE) {
        --m_madeCurrent;
        currentContext = nullptr;
    }

    if (m_supportsRobustnessExtension && testEGLContextLost())
        m_contextLost = true;

    return result;
}

bool EGLOffscreenContext::doneCurrent()
{
    assert(currentContext == this);

    if (--m_madeCurrent > 0)
        return true;

    currentContext = nullptr;

    bool success = true;
    if (m_detachedContext != EGL_NO_CONTEXT)
        success = ::eglMakeCurrent(m_eglDisplay, m_detachedDrawSurface, m_detachedReadSurface, m_detachedContext);

    m_detachedContext = 0;
    m_detachedReadSurface = 0;
    m_detachedDrawSurface = 0;
    return success;
}

void EGLOffscreenContext::copyFromTexture(GLTextureHandle& texture, bool flip)
{
    assert(currentContext == this);

    copyFromTextureGL(texture.textureId(), flip);
    glFinish();
}

void EGLOffscreenContext::initialize(EGLContext shareContext)
{
    if (m_eglContext != EGL_NO_CONTEXT)
        return;

    EGLint contextRobustnessAttributes[] = { 
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_CONTEXT_OPENGL_RESET_NOTIFICATION_STRATEGY_EXT, EGL_LOSE_CONTEXT_ON_RESET_EXT,
        EGL_NONE 
    };

    m_eglContext = ::eglCreateContext(m_eglDisplay, m_eglConfig, shareContext, contextRobustnessAttributes);

    if (m_eglContext != EGL_NO_CONTEXT) {
        if (makeCurrent()) {
            if (GLExtensions::supportsExtension(L"GL_EXT_robustness"))
                m_supportsRobustnessExtension = true;
            doneCurrent();
        }
        else
            eglDestroyContext(m_eglDisplay, m_eglContext);
    }

    if (m_eglContext == EGL_NO_CONTEXT) {
        EGLint contextAttributes[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
        };
        m_eglContext = ::eglCreateContext(m_eglDisplay, m_eglConfig, shareContext, contextAttributes);
        assert(m_eglContext != EGL_NO_CONTEXT);
    }

    if (makeCurrent()) {
        initializeShaders();
        doneCurrent();
    }
}

// Code from Webkit (https://webkit.org/) under LGPL v2 and BSD licenses (https://webkit.org/licensing-webkit/)
#define STRINGIFY(...) #__VA_ARGS__

static GLuint loadShader(GLenum type, const GLchar *shaderSrc)
{
    GLuint shader;
    GLint compiled;

    shader = glCreateShader(type);
    if (!shader)
        return 0;

    glShaderSource(shader, 1, &shaderSrc, 0);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

void EGLOffscreenContext::initializeShaders()
{
    if (m_shaderProgram)
        return;

    GLchar vShaderStr[] =
        STRINGIFY(
            attribute highp vec2 vertex;
            attribute highp vec2 textureCoordinates;
            varying highp vec2 textureCoords;
            void main(void)
            {
                gl_Position = vec4(vertex, 0.0, 1.0);
                textureCoords = textureCoordinates;
            }
        );

    GLchar fShaderStr[] =
        STRINGIFY(
            varying highp vec2 textureCoords;
            uniform sampler2D texture;
            void main(void)
            {
                gl_FragColor = texture2D(texture, textureCoords);
            }
        );

    GLuint vertexShader;
    GLuint fragmentShader;
    GLint linked;

    vertexShader = loadShader(GL_VERTEX_SHADER, vShaderStr);
    fragmentShader = loadShader(GL_FRAGMENT_SHADER, fShaderStr);
    if (!vertexShader || !fragmentShader)
        return;

    m_shaderProgram = glCreateProgram();
    if (!m_shaderProgram)
        return;

    glAttachShader(m_shaderProgram, vertexShader);
    glAttachShader(m_shaderProgram, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glLinkProgram(m_shaderProgram);
    glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &linked);
    if (!linked) {
        glDeleteProgram(m_shaderProgram);
        m_shaderProgram = 0;
    }

    m_vertexAttr = glGetAttribLocation(m_shaderProgram, "vertex");
    m_textureCoordinatesAttr = glGetAttribLocation(m_shaderProgram, "textureCoordinates");
    m_textureUniform = glGetAttribLocation(m_shaderProgram, "texture");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void EGLOffscreenContext::copyFromTextureGL(uint32_t texture, bool flip)
{
    glUseProgram(m_shaderProgram);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    GLfloat afVertices[] = {
        -1, -1,
        1, -1,
        -1,  1,
        1,  1
    };
    glVertexAttribPointer(m_vertexAttr, 2, GL_FLOAT, GL_FALSE, 0, afVertices);

    GLfloat aftextureCoordinates[] = {
        0, 0,
        1, 0,
        0, 1,
        1, 1
    };

    if (flip) {
        aftextureCoordinates[1] = 1;
        aftextureCoordinates[3] = 1;
        aftextureCoordinates[5] = 0;
        aftextureCoordinates[7] = 0;
    }

    glVertexAttribPointer(m_textureCoordinatesAttr, 2, GL_FLOAT, GL_FALSE, 0, aftextureCoordinates);

    glUniform1i(m_textureUniform, 0);

    glEnableVertexAttribArray(m_vertexAttr);
    glEnableVertexAttribArray(m_textureCoordinatesAttr);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisableVertexAttribArray(m_vertexAttr);
    glDisableVertexAttribArray(m_textureCoordinatesAttr);

    glBindTexture(GL_TEXTURE_2D, 0);
}

} // namespace appkit
} // namespace opengl
} // namespace android
