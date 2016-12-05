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

#include "ContentFrame.h"

#include <android/opengl/appkit/GLTextureStreamHost.h>
#include <android/os/appkit/Thread.h>
#include <android/view/ViewHostWindow.h>

#include <GLES2/gl2.h>

namespace android {
namespace view {
namespace appkit {

ContentFrame::ContentFrame()
    : m_contentView(new ContentView)
    , m_surfaceView(new opengl::GLSurfaceView)
    , m_contentVisibility(View::INVISIBLE)
    , m_surfaceCreatedOnce(false)
    , m_handler(os::Handler::create())
{
    addView(m_surfaceView);
    addView(m_contentView);
    m_contentView->setRenderClient(this);
    m_surfaceView->setRenderer(this);

    GLTextureStreamHost::initializeOnce();
}

ContentFrame::~ContentFrame()
{
    m_contentView->setRenderClient(nullptr);
    m_surfaceView->setRenderer(nullptr);
    m_contentView.reset();
    m_surfaceView.reset();
}

std::shared_ptr<ContentFrame> ContentFrame::createPopup(Rect& clientRect)
{
    ViewHostWindow* hostWindow = view::ViewHostWindow::createPopup(clientRect);
    if (!hostWindow)
        return nullptr;

    std::shared_ptr<ContentFrame> contentFrame = create();
    hostWindow->setContentView(contentFrame);
    return contentFrame;
}

void ContentFrame::invalidate()
{
    if (m_contentVisibility != View::VISIBLE)
        return;

    m_surfaceView->requestRender();
}

void ContentFrame::invalidate(Rect&)
{
    invalidate();
}

void ContentFrame::renderListChanged(std::shared_ptr<RenderContextList> renderList)
{
    m_pendingRenderList = std::move(renderList);
    m_surfaceView->requestRender();
}

void ContentFrame::renderEventPosted(std::function<void ()> r)
{
    m_surfaceView->queueEvent(std::move(r));
}

void ContentFrame::contentVisibilityChanged(int32_t visibility)
{
    if (visibility == m_contentVisibility)
        return;

    m_contentVisibility = visibility;

    if (visibility != View::VISIBLE)
        m_pendingRenderList.reset();

    m_surfaceView->requestRender();
}

void ContentFrame::contentInvalidated()
{
    invalidate();
}

void ContentFrame::onSurfaceCreated(opengl::GL10 gl)
{
    if (m_surfaceCreatedOnce) {
        if (m_currentRenderList)
            m_currentRenderList->contextLost();
    } else {
        m_surfaceCreatedOnce = true;
    }
}

void ContentFrame::onSurfaceChanged(opengl::GL10 gl, int32_t width, int32_t height)
{
}

void ContentFrame::destroyRenderListOnMainThread()
{
    if (m_currentRenderList) {
        std::shared_ptr<RenderContextList> protect(m_currentRenderList);
        m_handler->post([protect] {});
    }
}

void ContentFrame::onDrawFrame(opengl::GL10 gl)
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    if (m_contentVisibility != View::VISIBLE) {
        destroyRenderListOnMainThread();
        m_currentRenderList.reset();
    }

    if (m_pendingRenderList) {
        destroyRenderListOnMainThread();
        m_currentRenderList = std::move(m_pendingRenderList);
    }

    if (!m_currentRenderList)
        return;

    m_currentRenderList->paint();
}

} // namespace appkit
} // namespace view
} // namespace android
