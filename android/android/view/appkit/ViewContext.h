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

#include <android/content/res/Configuration.h>
#include <android/view/appkit/MotionDetector.h>

namespace android {
namespace view {
namespace appkit {

class ContentView;
class EditableContext;
class RenderContext;
class UIEvent;

class ViewContext {
public:
    virtual ~ViewContext() = default;

    virtual void attached(ContentView&) = 0;
    virtual void onDetachedFromWindow() = 0;

    virtual void setVisibility(bool) = 0;
    virtual void setFocus(bool) = 0;
    virtual void setActive(bool) = 0;
    virtual bool isVisible() = 0;
    virtual bool isFocused() = 0;
    virtual bool isActive() = 0;

    virtual void onLayout(int32_t, int32_t) = 0;
    virtual void displayLocation(int32_t, int32_t) = 0;
    virtual void onConfigurationChanged(Configuration& config) = 0;

    virtual void pause() = 0;
    virtual void resume() = 0;

    virtual MotionDetector::Settings motionSettings() = 0;

    virtual void sendSingleTap(float x, float y) = 0;
    virtual void sendDoubleTap(float x, float y) = 0;

    virtual bool sendGenericEvent(UIEvent&) = 0;

    virtual std::shared_ptr<EditableContext> editable() = 0;

    virtual int32_t width() = 0;
    virtual int32_t height() = 0;

    virtual int32_t contentWidth() = 0;
    virtual int32_t contentHeight() = 0;

    virtual float currentScale() = 0;
    virtual float minimumScale() = 0;
    virtual float maximumScale() = 0;
    
    virtual void scrollBegin(float x, float y) = 0;
    virtual bool scrollBy(float deltaX, float deltaY) = 0;
    virtual bool scrollTo(float x, float y) = 0;
    virtual void scrollEnd(float x, float y) = 0;

    virtual void pinchBegin(float x, float y) = 0;
    virtual void pinchTo(RectF& rect) = 0;
    virtual void pinchBy(float x, float y, float deltaScale) = 0;
    virtual void pinchEnd() = 0;

    virtual RectF viewport() = 0;

    virtual void invalidate() = 0;
    virtual void invalidate(Rect&) = 0;

    virtual std::shared_ptr<RenderContext> renderer() = 0;

protected:
    ViewContext() = default;
};

} // namespace appkit
} // namespace view
} // namespace android

using ViewContext = android::view::appkit::ViewContext;
