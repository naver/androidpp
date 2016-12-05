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

#include <android/graphics/RectF.h>
#include <android/view/appkit/UIEvent.h>

#include <memory>

namespace android {
namespace view {
namespace appkit {

class ANDROID_EXPORT MotionDetector {
public:
    union Settings {
        struct Flags {
            bool disableDetection : 1;
            bool canHandlePinchBy : 1;
            bool mouseTouchEvents : 1;
        };
        Flags flags;
        uint32_t value;

        Settings(uint32_t f = 0) : value(f) { }
    };

    class Client {
    public:
        virtual Settings clientSettings() = 0;
        virtual int32_t clientWidth() = 0;
        virtual int32_t clientHeight() = 0;
        virtual float clientScaleFactor() = 0;
        virtual float clientMinimumScaleFactor() = 0;
        virtual float clientMaximumScaleFactor() = 0;
        virtual int32_t contentWidth() = 0;
        virtual int32_t contentHeight() = 0;
        virtual RectF contentViewport() = 0;

        virtual void handleSingleTap(float x, float y) = 0;
        virtual void handleSingleTapUnconfirmed(float x, float y) = 0;
        virtual void handleDoubleTap(float x, float y) = 0;
        virtual void handleLongTap(float x, float y) = 0;

        virtual void handleShowPressState(float x, float y) = 0;
        virtual void handleShowPressCancel(float x, float y) = 0;
        virtual void handleLongPress(float x, float y) = 0;

        virtual void handleScrollBegin(float x, float y) = 0;
        virtual void handleScrollBy(float x, float y, float deltaX, float deltaY) = 0;
        virtual void handleScrollEnd(float x, float y) = 0;

        virtual void handleFlingStart(float x, float y, int32_t vx, int32_t vy) = 0;
        virtual void handleFlingCancel() = 0;

        virtual void handlePinchBegin(float x, float y) = 0;
        virtual void handlePinchBy(float x, float y, float deltaScale) = 0;
        virtual void handlePinchTo(RectF& rect) = 0;
        virtual void handlePinchEnd() = 0;

        virtual bool handleUnconfirmed(UIEvent&) = 0;
    };

    enum class State {
        Default,
        PinchToZoom,
    };

    MotionDetector(Client&);
    ~MotionDetector();

    bool sendGenericEvent(UIEvent&);

    void reset();

private:
    bool isDisabled() { return m_clientSettings.flags.disableDetection; }

    void clearState();

    bool processStateTransitionEvent(UIEvent&);

    bool processMouseEvent(UIEvent&);
    bool processWheelEvent(UIEvent&);
    bool processKeyEvent(UIEvent&);

    PointF translateScrollBy(float, float);
    RectF translatePinchBy(float, float, float);
    bool translateAccelerator(UIEvent&);

    Client& m_client;
    Settings m_clientSettings;
    State m_state;
};

} // namespace appkit
} // namespace view
} // namespace android

using MotionDetector = android::view::appkit::MotionDetector;
