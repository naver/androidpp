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

#include <android/view/View.h>

namespace android {
namespace opengl {

typedef void* GL10;

class ANDROID_EXPORT GLSurfaceView : public view::View {
public:
    static const bool DEBUG_GL = false;
    static const bool PRINT_ALL_CONFIGS = false;

    class Renderer {
    public:
        virtual ~Renderer() { }

        virtual void onSurfaceCreated(GL10 gl) = 0;
        virtual void onSurfaceChanged(GL10 gl, int32_t width, int32_t height) = 0;
        virtual void onDrawFrame(GL10 gl) = 0;
    };

    GLSurfaceView();
    virtual ~GLSurfaceView();

    // Control whether the EGL context is preserved when the GLSurfaceView is paused and resumed. 
    virtual void setPreserveEGLContextOnPause(bool preserveOnPause);
    virtual bool getPreserveEGLContextOnPause();
    // Set the renderer associated with this view. 
    virtual void setRenderer(Renderer* renderer);

    // Request that the renderer render a frame. 
    virtual void requestRender();

    // This is called immediately after any structural changes (format or size) have been made to the surface.
    virtual void surfaceChanged(int32_t format, int32_t width, int32_t height);
    // This is called immediately after the surface is first created.
    virtual void surfaceCreated();
    // This is called immediately before a surface is being destroyed.
    virtual void surfaceDestroyed();

    // Called after onRestoreInstanceState(Bundle), onRestart(), or onPause(), for your activity to start interacting with the user. 
    virtual void onResume();
    // Called as part of the activity lifecycle when an activity is going into the background, but has not (yet) been killed. 
    virtual void onPause();

    // Queue a runnable to be run on the GL rendering thread. 
    virtual void queueEvent(std::function<void ()> r);

    // view.View
    virtual void onAttachedToWindow() override;
    virtual void onDetachedFromWindow() override;
    virtual void onLayout(Rect&) override;

private:
    class GLThread;
    friend class GLThread;

    Renderer* m_renderer;
    std::unique_ptr<GLThread> m_glThread;
};

} // namespace opengl
} // namespace android

using GLSurfaceView = android::opengl::GLSurfaceView;
