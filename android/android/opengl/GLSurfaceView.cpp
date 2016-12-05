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

#include "GLSurfaceView.h"

#include <platforms/LogHelper.h>
#include <android/opengl/GLES2/esUtil.h>
#include <android/view/ViewHostWindow.h>
#include <android/view/ViewPrivate.h>

#include <assert>
#include <deque>

namespace android {
namespace opengl {

class GLThreadManager : public Object {
public:
    static GLThreadManager& shared()
    {
        static GLThreadManager threadManager;
        return threadManager;
    }

    bool shouldReleaseEGLContextWhenPausing()
    {
#ifdef NDEBUG
        return false;
#else
        return true;
#endif
    }

private:
    GLThreadManager() { }
};

static GLThreadManager& sGLThreadManager = GLThreadManager::shared();

class EGLHelper : public ESContext {
public:
    EGLHelper(GLSurfaceView& v) : m_view(v) {}

    bool begin()
    {
        if (!eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext))
            return false;
        glViewport(0, 0, width, height);
        return true;
    }

    int32_t swap()
    {
        if (!eglSwapBuffers(eglDisplay, eglSurface))
            return eglGetError();
        return EGL_SUCCESS;
    }

    void start()
    {
        if (eglContext != EGL_NO_CONTEXT)
            return;

        hWnd = (EGLNativeWindowType)view::getPrivate(m_view).hostWindow()->windowHandle();
        GLboolean hardwareAccelerated = esCreateContext(this, m_view.getWidth(), m_view.getHeight(), 8, 8, 8,
            false, true, true, false);

        if (!hardwareAccelerated) {
            LOGA("EGLContext creation failed");
            assert(false);
            esDestroyGL(this);
        }

        userData = (hardwareAccelerated) ? this : 0;
    }

    bool createSurface()
    {
        return esCreateSurface(this, width, height);
    }

    void destroySurface()
    {
        esDestroySurface(this);
    }

    void finish()
    {
        esDestroyContext(this);
        esTerminateDisplay(this);
    }

private:
    GLSurfaceView& m_view;
};

class GLSurfaceView::GLThread {
public:
    GLThread(GLSurfaceView& v)
        : m_view(v)
        , m_eglHelper(v)
        , m_shouldExit(false)
        , m_exited(false)
        , m_requestPause(false)
        , m_paused(false)
        , m_sizeChanged(true)
        , m_hasSurface(false)
        , m_surfaceIsBad(false)
        , m_waitingForSurface(false)
        , m_hasEglContext(false)
        , m_hasEglSurface(false)
        , m_requestRender(false)
        , m_renderComplete(false)
    { }

    ~GLThread()
    {
    }

    void requestRender()
    {
        synchronized (sGLThreadManager) {
            m_requestRender = true;
            sGLThreadManager.notifyAll();
        }
    }

    void surfaceCreated()
    {
        synchronized (sGLThreadManager) {
            m_hasSurface = true;
            m_finishedCreatingEglSurface = false;
            sGLThreadManager.notifyAll();
            while (m_waitingForSurface
                && !m_finishedCreatingEglSurface
                && !m_exited) {
                sGLThreadManager.wait();
            }
        }
    }

    void surfaceDestroyed()
    {
        synchronized (sGLThreadManager) {
            m_hasSurface = false;
            sGLThreadManager.notifyAll();
            while (!m_waitingForSurface
                && !m_exited) {
                sGLThreadManager.wait();
            }
        }
    }

    void onPause()
    {
        synchronized (sGLThreadManager) {
            m_requestPause = true;
            sGLThreadManager.notifyAll();
            while (!m_paused && !m_exited) {
                sGLThreadManager.wait();
            }
        }
    }

    void onResume()
    {
        synchronized (sGLThreadManager) {
            m_requestPause = false;
            m_requestRender = true;
            m_renderComplete = false;
            sGLThreadManager.notifyAll();
            while (m_paused && !m_exited
                && !m_renderComplete) {
                sGLThreadManager.wait();
            }
        }
    }

    void onWindowResize(int32_t w, int32_t h)
    {
        synchronized (sGLThreadManager) {
            m_eglHelper.width = w;
            m_eglHelper.height = h;
            m_sizeChanged = true;
            m_requestRender = true;
            m_renderComplete = false;
            sGLThreadManager.notifyAll();
            while (!m_paused && !m_exited
                && !m_renderComplete
                && !m_surfaceIsBad) {
                sGLThreadManager.wait();
            }
        }
    }

    void requestExitAndWait()
    {
        synchronized (sGLThreadManager) {
            m_shouldExit = true;
            sGLThreadManager.notifyAll();
            while (!m_exited) {
                sGLThreadManager.wait();
            }
        }
    }

    // Threading
    void run()
    {
        if (m_shouldExit || m_exited)
            return;

        m_thread = std::unique_ptr<std::thread>(new std::thread([=] {
            readyToRun();
            threadLoop();
        }));
    }

    void requestExit()
    {
        requestExitAndWait();
        m_thread->join();
    }

    void readyToRun()
    {
        esInitContext(&m_eglHelper);
    }

    void onAttachedToWindow()
    {
        view::getPrivate(m_view).hostWindow()->setOnWindowPositionChangeListener([this](bool change) {
            if (change)
               suspendPaint();
            else
               restartPaint();
        });
    }

    void onDetachedFromWindow()
    {
        view::getPrivate(m_view).hostWindow()->setOnWindowPositionChangeListener(nullptr);
    }

    void suspendPaint()
    {
        synchronized (sGLThreadManager) {
            m_surfaceIsBad = true;
            m_renderComplete = false;
            sGLThreadManager.notifyAll();
            while (!m_exited && !m_renderComplete) {
                sGLThreadManager.wait();
            }
        }
    }

    void restartPaint()
    {
        synchronized (sGLThreadManager) {
            m_surfaceIsBad = false;
            m_requestRender = true;
            m_renderComplete = false;
            sGLThreadManager.notifyAll();
            while (!m_exited && !m_renderComplete) {
                sGLThreadManager.wait();
            }
        }
    }

private:
    void threadLoop() 
    {
        guardedRun();
        synchronized (sGLThreadManager) {
            stopEglSurfaceLocked();
            stopEglContextLocked();
            m_exited = true;
            sGLThreadManager.threadExiting();
        }
    }

    void stopEglSurfaceLocked()
    {
        if (m_hasEglSurface) {
            m_hasEglSurface = false;
            m_eglHelper.destroySurface();
        }
    }

    void stopEglContextLocked()
    {
        if (m_hasEglContext) {
            m_eglHelper.finish();
            m_hasEglContext = false;
        }
    }

    bool readyToDraw()
    {
        return !m_paused && m_hasSurface && !m_surfaceIsBad && m_requestRender;
    }

    bool testEGLContextLost()
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

    void guardedRun()
    {
        GL10 gl = nullptr;
        bool createEglContext = false;
        bool lostEglContext = false;
        bool sizeChanged = false;
        bool createEglSurface = false;
        int32_t w = 0;
        int32_t h = 0;
        std::function<void ()>* event = nullptr;

        while (true) {
            synchronized (sGLThreadManager) {
                while (true) {
                    if (m_shouldExit)
                        return;

                    if (testEGLContextLost())
                        lostEglContext = true;

                    if (UNLIKELY(!m_eventQueue.empty())) {
                        event = &m_eventQueue.front();
                        break;
                    }

                    bool pausing = false;
                    if (m_paused != m_requestPause) {
                        pausing = m_requestPause;
                        m_paused = m_requestPause;
                        sGLThreadManager.notifyAll();
                    }

                    if (lostEglContext) {
                        stopEglSurfaceLocked();
                        stopEglContextLocked();
                        lostEglContext = false;
                    }

                    if (pausing && m_hasEglSurface)
                        stopEglSurfaceLocked();

                    if (pausing && m_hasEglContext) {
                        bool preserveEglContextOnPause = m_view.getPreserveEGLContextOnPause();
                        if (!preserveEglContextOnPause
                            || sGLThreadManager.shouldReleaseEGLContextWhenPausing()) {
                            stopEglContextLocked();
                        }
                    }

                    if (!m_hasSurface && !m_waitingForSurface) {
                        if (m_hasEglSurface)
                            stopEglSurfaceLocked();

                        m_waitingForSurface = true;
                        m_surfaceIsBad = false;
                        sGLThreadManager.notifyAll();
                    }
    
                    if (m_hasSurface && m_waitingForSurface) {
                        m_waitingForSurface = false;
                        sGLThreadManager.notifyAll();
                    }

                    if (!m_renderComplete) {
                        m_renderComplete = true;
                        sGLThreadManager.notifyAll();
                    }

                    if (readyToDraw()) {
                        // If we don't have an EGL context, try to
                        // acquire one.
                        if (!m_hasEglContext) {
                            m_eglHelper.start();
                            m_hasEglContext = true;
                            createEglContext = true;
                            sGLThreadManager.notifyAll();
                        }

                        if (m_hasEglContext && !m_hasEglSurface) {
                            m_hasEglSurface = true;
                            createEglSurface = true;
                            sizeChanged = true;
                        }

                        if (m_hasEglSurface) {
                            // We must have EGL surface as well.
                            if (m_sizeChanged) {
                                sizeChanged = true;
                                w = m_eglHelper.width;
                                h = m_eglHelper.height;

                                createEglSurface = true;

                                m_sizeChanged = false;
                            }
                            m_requestRender = false;
                            sGLThreadManager.notifyAll();
                            break;
                        }
                    }

                    sGLThreadManager.wait();
                }
            } // end of synchronized (sGLThreadManager)

            if (UNLIKELY(event)) {
                (*event)();
                event = nullptr;
                m_eventQueue.pop_front();
                continue;
            }

            if (createEglSurface) {
                if (m_eglHelper.createSurface()) {
                    synchronized (sGLThreadManager) {
                        m_finishedCreatingEglSurface = true;
                        sGLThreadManager.notifyAll();
                    }
                }
                else {
                    synchronized (sGLThreadManager) {
                        m_finishedCreatingEglSurface = true;
                        m_surfaceIsBad = true;
                        sGLThreadManager.notifyAll();
                    }
                    continue;
                }
                createEglSurface = false;
            }

            gl = &m_eglHelper;
            if (createEglContext) {
                m_view.m_renderer->onSurfaceCreated(gl);
                createEglContext = false;
            }
            if (sizeChanged) {
                m_view.m_renderer->onSurfaceChanged(gl, w, h);
                sizeChanged = false;
            }
            {
                if (!m_eglHelper.begin())
                    continue;
                m_view.m_renderer->onDrawFrame(gl);
            }

            int32_t swapError = m_eglHelper.swap();
            switch (swapError) {
                case EGL_SUCCESS:
                    break;
                case EGL_CONTEXT_LOST:
                    lostEglContext = true;
                    break;
                default:
                    // Other errors typically mean that the current
                    // surface is bad,
                    // probably because the SurfaceView surface has been
                    // destroyed,
                    // but we haven't been notified yet.
                    // Log the error to help developers understand why
                    // rendering stopped.
                    synchronized (sGLThreadManager) {
                        m_surfaceIsBad = true;
                        sGLThreadManager.notifyAll();
                    }
                    break;
            }
        }
    }

public:
    void queueEvent(std::function<void ()> r)
    {
        if (!r) {
            LOGA("r must not be null");
            assert(false);
        }
        synchronized (sGLThreadManager) {
            m_eventQueue.push_back(std::move(r));
            sGLThreadManager.notifyAll();
        }
    }

private:
    bool m_shouldExit;
    bool m_exited;
    bool m_requestPause;
    bool m_paused;
    bool m_sizeChanged;
    bool m_hasSurface;
    bool m_finishedCreatingEglSurface;
    bool m_surfaceIsBad;
    bool m_waitingForSurface;
    bool m_hasEglContext;
    bool m_hasEglSurface;
    bool m_requestRender;
    bool m_renderComplete;
    std::deque<std::function<void ()>> m_eventQueue;

    GLSurfaceView& m_view;
    EGLHelper m_eglHelper;
    std::unique_ptr<std::thread> m_thread;
};

GLSurfaceView::GLSurfaceView()
    : m_renderer(0)
    , m_glThread(new GLThread(*this))
{
}

GLSurfaceView::~GLSurfaceView()
{
    if (m_renderer)
        m_glThread->requestExit();
}

// Control whether the EGL context is preserved when the GLSurfaceView is paused and resumed. 
void GLSurfaceView::setPreserveEGLContextOnPause(bool preserveOnPause)
{
}

bool GLSurfaceView::getPreserveEGLContextOnPause()
{
    return false;
}

void GLSurfaceView::setRenderer(Renderer* renderer)
{
    if (renderer == m_renderer)
        return;

    // FIXME: May needs referencing.
    m_renderer = renderer;

    if (m_renderer)
        m_glThread->run();
    else
        m_glThread->requestExit();
}

void GLSurfaceView::requestRender()
{
    m_glThread->requestRender();
}

// This is called immediately after any structural changes (format or size) have been made to the surface.
void GLSurfaceView::surfaceChanged(int32_t format, int32_t width, int32_t height)
{
    m_glThread->onWindowResize(width, height);
}

// This is called immediately after the surface is first created.
void GLSurfaceView::surfaceCreated()
{
    m_glThread->onAttachedToWindow();
    m_glThread->surfaceCreated();
}

// This is called immediately before a surface is being destroyed.
void GLSurfaceView::surfaceDestroyed()
{
    m_glThread->surfaceDestroyed();
    m_glThread->onDetachedFromWindow();
}

// Called after onRestoreInstanceState(Bundle), onRestart(), or onPause(), for your activity to start interacting with the user. 
void GLSurfaceView::onResume()
{
    m_glThread->onResume();
}

// Called as part of the activity lifecycle when an activity is going into the background, but has not (yet) been killed. 
void GLSurfaceView::onPause()
{
    m_glThread->onPause();
}

void GLSurfaceView::queueEvent(std::function<void ()> r)
{
    m_glThread->queueEvent(std::move(r));
}

void GLSurfaceView::onAttachedToWindow()
{
    View::onAttachedToWindow();
    surfaceCreated();
}

void GLSurfaceView::onDetachedFromWindow()
{
    View::onDetachedFromWindow();
    surfaceDestroyed();
}

void GLSurfaceView::onLayout(Rect& rc)
{
    View::onLayout(rc);
    surfaceChanged(0, rc.width(), rc.height());
}

} // namespace opengl
} // namespace android
