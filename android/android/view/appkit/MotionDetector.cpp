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

#include "MotionDetector.h"

#include <android/view/WindowProvider.h>
#include <android/view/appkit/UIEvent.h>

#include <algorithm>

namespace android {
namespace view {
namespace appkit {

MotionDetector::MotionDetector(Client& c)
    : m_client(c)
    , m_state(State::Default)
{
}

MotionDetector::~MotionDetector()
{
}

bool MotionDetector::sendGenericEvent(UIEvent& event)
{
    if (isDisabled())
        return m_client.handleUnconfirmed(event);

    if (processStateTransitionEvent(event))
        return true;

    switch (event.source()) {
    case UIEvent::Source::Mouse:
        return processMouseEvent(event);
    case UIEvent::Source::Wheel:
        return processWheelEvent(event);
    case UIEvent::Source::Key:
        return processKeyEvent(event);
    default:
        break;
    };

    return m_client.handleUnconfirmed(event);
}

void MotionDetector::reset()
{
    m_clientSettings = m_client.clientSettings();

    if (isDisabled())
        clearState();
}

void MotionDetector::clearState()
{
    switch (m_state) {
    case State::PinchToZoom:
        m_state = State::Default;
        m_client.handlePinchEnd();
        return;
    case State::Default:
    default:
        return;
    }
}

bool MotionDetector::processStateTransitionEvent(UIEvent& event)
{
    switch (m_state) {
    case State::Default:
        if (event.source() == UIEvent::Source::Wheel && event.modifiers().flags.ctrlKey) {
            m_state = State::PinchToZoom;
            const UIEvent::Pointer& pointer = *event.pointers();
            const PointF& coord = pointer.coord;
            m_client.handlePinchBegin(coord.x, coord.y);
            return false;
        }
        break;
    case State::PinchToZoom:
        if (event.source() == UIEvent::Source::Key && event.action() == UIEvent::Action::Up
            && event.keyState().virtualKeyCode == VK_CONTROL) {
            m_state = State::Default;
            m_client.handlePinchEnd();
            return false;
        }
        break;
    default:
        break;
    };
    return false;
}

bool MotionDetector::processMouseEvent(UIEvent& event)
{
    if (!m_clientSettings.flags.mouseTouchEvents)
        return m_client.handleUnconfirmed(event);

    const UIEvent::Pointer& pointer = *event.pointers();
    const PointF& coord = pointer.coord;

    if (event.action() == UIEvent::Action::Up && event.modifiers().flags.leftButton)
        if (event.repeatCount() == 1) {
            m_client.handleSingleTap(coord.x, coord.y);
            return true;
        } else if (event.repeatCount() == 2) {
            m_client.handleDoubleTap(coord.x, coord.y);
            return true;
        }

    return m_client.handleUnconfirmed(event);
}

bool MotionDetector::processWheelEvent(UIEvent& event)
{
    static const float pixelsPerLineStep = 40.f;
    static const float scalePerWheelTick = 1.1f;

    const UIEvent::Pointer& pointer = *event.pointers();
    const PointF& coord = pointer.coord;
    const int wheelTick = static_cast<int>(pointer.magnitude / WHEEL_DELTA);

    switch (m_state) {
    case State::Default: {
        const PointF& globalCoord = pointer.globalCoord;
        PointF scrollDelta = translateScrollBy(0.0f, -wheelTick * pixelsPerLineStep);
        m_client.handleScrollBegin(coord.x, coord.y);
        m_client.handleScrollBy(coord.x, coord.y, scrollDelta.x, scrollDelta.y);
        m_client.handleScrollEnd(coord.x, coord.y);
        return true;
    }
    case State::PinchToZoom: {
        float deltaScale = pow(scalePerWheelTick, wheelTick);
        if (m_clientSettings.flags.canHandlePinchBy)
            m_client.handlePinchBy(coord.x, coord.y, deltaScale);
        else
            m_client.handlePinchTo(translatePinchBy(coord.x, coord.y, deltaScale));
        return true;
    }
    default:
        break;
    }

    return m_client.handleUnconfirmed(event);
}

bool MotionDetector::processKeyEvent(UIEvent& event)
{
    switch (m_state) {
    case State::Default:
        if (translateAccelerator(event))
            return true;
        break;
    default:
        break;
    }

    return m_client.handleUnconfirmed(event);
}

PointF MotionDetector::translateScrollBy(float deltaX, float deltaY)
{
    float clientScaleFactor = m_client.clientScaleFactor();
    return PointF(deltaX / clientScaleFactor, deltaY / clientScaleFactor);
}

RectF MotionDetector::translatePinchBy(float anchorX, float anchorY, float deltaScale)
{
    RectF contentViewport = m_client.contentViewport();
    float oldScale = m_client.clientScaleFactor();
    float newScale = std::max(m_client.clientMinimumScaleFactor(), std::min(m_client.clientMaximumScaleFactor(), oldScale * deltaScale));

    float newLeft = contentViewport.left + anchorX / oldScale - anchorX / newScale;
    float newTop = contentViewport.top + anchorY / oldScale - anchorY / newScale;    
    float newRight = newLeft + m_client.clientWidth() / newScale;
    float newBottom = newTop + m_client.clientHeight() / newScale;

    return RectF(newLeft, newTop, newRight, newBottom);
}

bool MotionDetector::translateAccelerator(UIEvent& event)
{
    return false;
}

} // namespace appkit
} // namespace view
} // namespace android
