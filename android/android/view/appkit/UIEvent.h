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

#include <android/graphics/PointF.h>

namespace android {
namespace view {
namespace appkit {

class ANDROID_EXPORT UIEvent {
public:
    enum class Source {
        Unknown,
        Mouse,
        Wheel,
        Touch,
        Key,
    };

    enum class Action {
        None,
        Cancel,
        Down,
        Up,
        Press,
        Move,
        Enter,
        Leave,
        Away,
    };

    union Modifiers {
        struct Flags {
            bool altKey : 1;
            bool ctrlKey : 1;
            bool shiftKey : 1;
            bool leftButton : 1;
            bool rightButton : 1;
            bool middleButton : 1;
        };
        Flags flags;
        uint32_t value;

        Modifiers(uint32_t f = 0) : value(f) { }
    };

    struct Pointer {
        int32_t id;
        PointF coord;
        PointF globalCoord;
        float size;
        float orientation;
        float magnitude;

        Pointer()
            : id(0)
            , size(0.0f)
            , orientation(0.0f)
            , magnitude(0.0f)
        {
        }
        Pointer(int32_t id, PointF coord, PointF globalCoord, float size = 0.0f, float orientation = 0.0f, float magnitude = 0.0f)
            : id(id)
            , coord(coord)
            , globalCoord(globalCoord)
            , size(size)
            , orientation(orientation)
            , magnitude(magnitude)
        {
        }
    };

    struct KeyState {
        uint32_t nativeKeyCode;
        uint32_t virtualKeyCode;
        wchar_t unicodeCharacter;
        bool isAutoRepeat;
        bool isSystemKey;
        bool isKeypadKey;
        size_t strokeHash;

        KeyState()
            : nativeKeyCode(0)
            , virtualKeyCode(0)
            , unicodeCharacter(0)
            , isAutoRepeat(false)
            , isSystemKey(false)
            , isKeypadKey(false)
            , strokeHash(0)
        {
        }
        KeyState(uint32_t nativeKeyCode, uint32_t virtualKeyCode, wchar_t unicodeCharacter, bool isAutoRepeat, bool isSystemKey, bool isKeypadKey, size_t strokeHash)
            : nativeKeyCode(nativeKeyCode)
            , virtualKeyCode(virtualKeyCode)
            , unicodeCharacter(unicodeCharacter)
            , isAutoRepeat(isAutoRepeat)
            , isSystemKey(isSystemKey)
            , isKeypadKey(isKeypadKey)
            , strokeHash(strokeHash)
        {
        }
    };

    static std::chrono::milliseconds currentTime()
    {
        return System::currentTimeMillis();
    }

    static UIEvent create()
    {
        return UIEvent(currentTime(), Source::Unknown, Action::None, Modifiers(), 0, 0);
    }
    static UIEvent create(Source source, Action action, Modifiers modifiers, uint32_t repeatCount)
    {
        return UIEvent(currentTime(), source, action, modifiers, repeatCount, 0);
    }
    static UIEvent create(std::chrono::milliseconds timestamp, Source source, Action action, Modifiers modifiers, uint32_t repeatCount)
    {
        return UIEvent(timestamp, source, action, modifiers, repeatCount, 0);
    }
    ~UIEvent() = default;

    std::chrono::milliseconds timestamp() { return m_timestamp; }

    Source source() { return m_source; }
    Action action() { return m_action; }
    Modifiers modifiers() { return m_modifiers; }
    uint32_t repeatCount() { return m_repeatCount; }

    uint32_t pointerCount() { return m_pointerCount; }
    virtual const Pointer* pointers() { return 0; }

    virtual const KeyState& keyState() { static const KeyState defaultState; return defaultState; }

protected:
    UIEvent(std::chrono::milliseconds timestamp, Source source, Action action, Modifiers modifiers, uint32_t repeatCount, uint32_t pointerCount)
        : m_timestamp(timestamp)
        , m_source(source)
        , m_action(action)
        , m_modifiers(modifiers)
        , m_repeatCount(repeatCount)
        , m_pointerCount(pointerCount)
    {
    }

private:
    std::chrono::milliseconds m_timestamp;
    Source m_source;
    Action m_action;
    Modifiers m_modifiers;
    uint32_t m_repeatCount;
    uint32_t m_pointerCount;
};

static inline bool isClickEvent(UIEvent& event)
{
    if (event.source() != UIEvent::Source::Mouse)
        return false;
    if (event.action() != UIEvent::Action::Up)
        return false;
    if (event.repeatCount() != 1)
        return false;
    if (!event.modifiers().flags.leftButton)
        return false;

    return true;
}

} // namespace appkit
} // namespace view
} // namespace android

using UIEvent = android::view::appkit::UIEvent;
