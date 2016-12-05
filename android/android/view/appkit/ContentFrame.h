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

#include <android/opengl/GLSurfaceView.h>
#include <android/view/ViewGroup.h>
#include <android/view/appkit/ContentView.h>
#include <android/view/appkit/RenderContextList.h>

namespace android {
namespace view {
namespace appkit {

class RenderContext;

class ContentFrame : public ViewGroup
    , public ContentView::RenderClient
    , public GLSurfaceView::Renderer {
public:
    ANDROID_EXPORT static std::shared_ptr<ContentFrame> create()
    {
        return std::shared_ptr<ContentFrame>(new ContentFrame());
    }
    ANDROID_EXPORT static std::shared_ptr<ContentFrame> createPopup(Rect& clientRect);
    ANDROID_EXPORT virtual ~ContentFrame();

    ANDROID_EXPORT ContentView& contentView() { return *m_contentView; }

    ANDROID_EXPORT virtual void invalidate() override;
    ANDROID_EXPORT virtual void invalidate(Rect&) override;

    // ContentView.RenderClient
    ANDROID_EXPORT virtual void renderListChanged(std::shared_ptr<RenderContextList>) override;
    ANDROID_EXPORT virtual void renderEventPosted(std::function<void ()>) override;
    ANDROID_EXPORT virtual void contentVisibilityChanged(int32_t) override;
    ANDROID_EXPORT virtual void contentInvalidated() override;

    // opengl.GLSurfaceView.Renderer
    ANDROID_EXPORT virtual void onSurfaceCreated(opengl::GL10 gl) override;
    ANDROID_EXPORT virtual void onSurfaceChanged(opengl::GL10 gl, int32_t width, int32_t height) override;
    ANDROID_EXPORT virtual void onDrawFrame(opengl::GL10 gl) override;

protected:
    ANDROID_EXPORT ContentFrame();

private:
    void destroyRenderListOnMainThread();

    std::shared_ptr<ContentView> m_contentView;
    std::shared_ptr<opengl::GLSurfaceView> m_surfaceView;
    std::shared_ptr<RenderContextList> m_currentRenderList;
    std::shared_ptr<RenderContextList> m_pendingRenderList;
    int32_t m_contentVisibility;
    bool m_surfaceCreatedOnce;
    std::shared_ptr<Handler> m_handler;
};

} // namespace appkit
} // namespace view
} // namespace android

using ContentFrame = android::view::appkit::ContentFrame;
