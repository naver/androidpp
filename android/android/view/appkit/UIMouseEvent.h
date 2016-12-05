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

#include <android/view/appkit/UIEvent.h>

namespace android {
namespace view {
namespace appkit {

class ANDROID_EXPORT UIMouseEvent : public UIEvent {
public:
    static UIMouseEvent create()
    {
        return UIMouseEvent(currentTime(), Action::None, Modifiers(), PointF(), PointF(), 1);
    }
    static UIMouseEvent create(Action action, Modifiers modifiers, PointF& coord, PointF& globalCoord, int repeatCount)
    {
        return UIMouseEvent(currentTime(), action, modifiers, coord, globalCoord, repeatCount);
    }
    static UIMouseEvent create(std::chrono::milliseconds timestamp, Action action, Modifiers modifiers, PointF& coord, PointF& globalCoord, int repeatCount)
    {
        return UIMouseEvent(timestamp, action, modifiers, coord, globalCoord, repeatCount);
    }
    ~UIMouseEvent() = default;

    const Pointer* pointers() override { return &m_pointer; }

protected:
    UIMouseEvent(std::chrono::milliseconds timestamp, Action action, Modifiers modifiers, PointF& coord, PointF& globalCoord, int repeatCount)
        : UIEvent(timestamp, Source::Mouse, action, modifiers, repeatCount, 1)
        , m_pointer(Pointer(0, coord, globalCoord))
    {
    }

private:
    Pointer m_pointer;
};

} // namespace appkit
} // namespace view
} // namespace android

using UIMouseEvent = android::view::appkit::UIMouseEvent;
