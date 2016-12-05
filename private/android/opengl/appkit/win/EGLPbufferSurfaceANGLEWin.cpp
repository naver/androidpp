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

#include <android/opengl/appkit/EGLPbufferSurface.h>

#include <EGL/eglext.h>

#include <assert>

namespace android {
namespace opengl {
namespace appkit {

bool EGLPbufferSurface::platformCreate(EGLContext shareContext)
{
    static PFNEGLQUERYSURFACEPOINTERANGLEPROC eglQuerySurfacePointerANGLE = 0;
    if (!eglQuerySurfacePointerANGLE) {
        eglQuerySurfacePointerANGLE = reinterpret_cast<PFNEGLQUERYSURFACEPOINTERANGLEPROC>(eglGetProcAddress("eglQuerySurfacePointerANGLE"));
        if (!eglQuerySurfacePointerANGLE)
            return false;
    }

    EGLint attrib_list[] = {
        EGL_WIDTH, m_width,
        EGL_HEIGHT, m_height,
        EGL_TEXTURE_FORMAT, EGL_TEXTURE_RGBA,
        EGL_TEXTURE_TARGET, EGL_TEXTURE_2D,
        EGL_NONE
    };

    m_eglSurface = eglCreatePbufferSurface(m_eglDisplay, m_eglConfig, attrib_list);
    if (m_eglSurface == EGL_NO_SURFACE)
        return false;

    void* pFrontBufferShareHandle;
    eglQuerySurfacePointerANGLE(m_eglDisplay, m_eglSurface, EGL_D3D_TEXTURE_2D_SHARE_HANDLE_ANGLE, &pFrontBufferShareHandle);
    m_shareHandle = (uintptr_t)pFrontBufferShareHandle;
    return true;
}

bool EGLPbufferSurface::platformImport()
{
    assert(m_shareHandle);

    EGLint attrib_list[] = {
        EGL_WIDTH, m_width,
        EGL_HEIGHT, m_height,
        EGL_TEXTURE_FORMAT, EGL_TEXTURE_RGBA,
        EGL_TEXTURE_TARGET, EGL_TEXTURE_2D,
        EGL_NONE
    };

    EGLSurface surface = eglCreatePbufferFromClientBuffer(m_eglDisplay, EGL_D3D_TEXTURE_2D_SHARE_HANDLE_ANGLE, 
        reinterpret_cast<EGLClientBuffer>(m_shareHandle), m_eglConfig, attrib_list);
    if (surface == EGL_NO_SURFACE) {
        assert(false);
        return false;
    }

    m_eglSurface = surface;
    return true;
}

} // namespace appkit
} // namespace opengl
} // namespace android
