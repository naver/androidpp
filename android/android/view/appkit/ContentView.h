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

#include <android/os/Handler.h>
#include <android/view/View.h>
#include <android/view/appkit/Cursor.h>
#include <android/view/appkit/EditableContext.h>
#include <android/view/appkit/MotionDetector.h>
#include <android/view/appkit/UIEvent.h>

#include <platforms/KeywordMacros.h>

namespace android {
namespace view {
namespace appkit {

class RenderContextList;
class ViewContext;

class ContentView : public View, public MotionDetector::Client {
public:
    class RenderClient {
    public:
        virtual ~RenderClient() { }

        virtual void renderListChanged(std::shared_ptr<RenderContextList>) = 0;
        virtual void renderEventPosted(std::function<void ()>) = 0;
        virtual void contentVisibilityChanged(int32_t) = 0;
        virtual void contentInvalidated() = 0;
    };

    ANDROID_EXPORT ContentView();
    ANDROID_EXPORT virtual ~ContentView();

    ANDROID_EXPORT void addView(std::shared_ptr<ViewContext>);
    ANDROID_EXPORT void removeView(std::shared_ptr<ViewContext>);
    ANDROID_EXPORT void moveAfter(std::shared_ptr<ViewContext>, std::shared_ptr<ViewContext>);
    ANDROID_EXPORT void bringChildToFront(std::shared_ptr<ViewContext>);
    ANDROID_EXPORT void sendToBack(std::shared_ptr<ViewContext>);

    ANDROID_EXPORT ViewContext* foregroundView();

    void setRenderClient(RenderClient*);

    ANDROID_EXPORT void motionSettingsChanged();
    ANDROID_EXPORT void contentInvalidated();

    ANDROID_EXPORT float deviceScaleFactor();

    ANDROID_EXPORT virtual void onPause();
    ANDROID_EXPORT virtual void onResume();

    ANDROID_EXPORT virtual bool sendGenericEvent(UIEvent&);

    ANDROID_EXPORT virtual bool startInputMethod();
    ANDROID_EXPORT virtual bool endInputMethod();

    ANDROID_EXPORT virtual std::shared_ptr<EditableContext> editableContext();

private:
    DECLARE_INNER_CLASS(ViewObserver);

    void detachViewFromRenderClient(std::shared_ptr<ViewContext>);
    void updateContentsRenderState();

    // View
    virtual void setVisibility(int32_t) override;

    virtual void onAttachedToWindow() override;
    virtual void onDetachedFromWindow() override;
    virtual void onFocusChanged(bool gainFocus, int32_t direction, Rect& previouslyFocusedRect) override;
    virtual void onMeasure(int32_t, int32_t) override;
    virtual void onLayout(Rect&) override;
    virtual void onConfigurationChanged(Configuration&) override;

    // MotionDetector.Client
    virtual MotionDetector::Settings clientSettings() override;
    virtual int32_t clientWidth() override;
    virtual int32_t clientHeight() override;
    virtual float clientScaleFactor() override;
    virtual float clientMinimumScaleFactor() override;
    virtual float clientMaximumScaleFactor() override;
    virtual int32_t contentWidth() override;
    virtual int32_t contentHeight() override;
    virtual RectF contentViewport() override;

    virtual void handleSingleTap(float x, float y) override;
    virtual void handleSingleTapUnconfirmed(float x, float y) override;
    virtual void handleDoubleTap(float x, float y) override;
    virtual void handleLongTap(float x, float y) override;

    virtual void handleShowPressState(float x, float y) override;
    virtual void handleShowPressCancel(float x, float y) override;
    virtual void handleLongPress(float x, float y) override;

    virtual void handleScrollBegin(float x, float y) override;
    virtual void handleScrollBy(float x, float y, float deltaX, float deltaY) override;
    virtual void handleScrollEnd(float x, float y) override;

    virtual void handleFlingStart(float x, float y, int32_t vx, int32_t vy) override;
    virtual void handleFlingCancel() override;

    virtual void handlePinchBegin(float x, float y) override;
    virtual void handlePinchTo(RectF& rect) override;
    virtual void handlePinchBy(float x, float y, float deltaScale) override;
    virtual void handlePinchEnd() override;

    virtual bool handleUnconfirmed(UIEvent&) override;

private:
    RenderClient* m_renderClient;
    std::vector<std::shared_ptr<ViewContext>> m_contents;
    MotionDetector m_motionDetector;
    std::shared_ptr<os::Handler> m_handler;
};

} // namespace appkit
} // namespace view
} // namespace android

using ContentView = android::view::appkit::ContentView;
