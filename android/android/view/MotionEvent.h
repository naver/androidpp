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
#include <android/view/InputEvent.h>

namespace android {
namespace view {

class MotionEventPrivate;

class MotionEvent final : public InputEvent {
    friend class MotionEventPrivate;
public:
    // Constant for getActionMasked(): A button has been pressed.
    static const int32_t ACTION_BUTTON_PRESS = 0x0000000b;
    // Constant for getActionMasked(): A button has been released.
    static const int32_t ACTION_BUTTON_RELEASE = 0x0000000c;
    // Constant for getActionMasked(): The current gesture has been aborted.
    static const int32_t ACTION_CANCEL = 0x00000003;
    // Constant for getActionMasked(): A pressed gesture has started, the motion contains the initial starting location.
    static const int32_t ACTION_DOWN = 0x00000000;
    // Constant for getActionMasked(): The pointer is not down but has entered the boundaries of a window or view.
    static const int32_t ACTION_HOVER_ENTER = 0x00000009;
    // Constant for getActionMasked(): The pointer is not down but has exited the boundaries of a window or view.
    static const int32_t ACTION_HOVER_EXIT = 0x0000000a;
    // Constant for getActionMasked(): A change happened but the pointer is not down (unlike ACTION_MOVE).
    static const int32_t ACTION_HOVER_MOVE = 0x00000007;
    // Bit mask of the parts of the action code that are the action itself.
    static const int32_t ACTION_MASK = 0x000000ff;
    // Constant for getActionMasked(): A change has happened during a press gesture (between ACTION_DOWN and ACTION_UP).
    static const int32_t ACTION_MOVE = 0x00000002;
    // Constant for getActionMasked(): A movement has happened outside of the normal bounds of the UI element.
    static const int32_t ACTION_OUTSIDE = 0x00000004;
    // This constant was deprecated in API level 8.Use ACTION_POINTER_INDEX_MASK to retrieve the data index associated with ACTION_POINTER_DOWN.
    static const int32_t ACTION_POINTER_1_DOWN = 0x00000005;
    // This constant was deprecated in API level 8.Use ACTION_POINTER_INDEX_MASK to retrieve the data index associated with ACTION_POINTER_UP.
    static const int32_t ACTION_POINTER_1_UP = 0x00000006;
    // This constant was deprecated in API level 8.Use ACTION_POINTER_INDEX_MASK to retrieve the data index associated with ACTION_POINTER_DOWN.
    static const int32_t ACTION_POINTER_2_DOWN = 0x00000105;
    // This constant was deprecated in API level 8.Use ACTION_POINTER_INDEX_MASK to retrieve the data index associated with ACTION_POINTER_UP.
    static const int32_t ACTION_POINTER_2_UP = 0x00000106;
    // This constant was deprecated in API level 8.Use ACTION_POINTER_INDEX_MASK to retrieve the data index associated with ACTION_POINTER_DOWN.
    static const int32_t ACTION_POINTER_3_DOWN = 0x00000205;
    // This constant was deprecated in API level 8.Use ACTION_POINTER_INDEX_MASK to retrieve the data index associated with ACTION_POINTER_UP.
    static const int32_t ACTION_POINTER_3_UP = 0x00000206;
    // Constant for getActionMasked(): A non-primary pointer has gone down.
    static const int32_t ACTION_POINTER_DOWN = 0x00000005;
    // This constant was deprecated in API level 8.Renamed to ACTION_POINTER_INDEX_MASK to match the actual data contained in these bits.
    static const int32_t ACTION_POINTER_ID_MASK = 0x0000ff00;
    // This constant was deprecated in API level 8.Renamed to ACTION_POINTER_INDEX_SHIFT to match the actual data contained in these bits.
    static const int32_t ACTION_POINTER_ID_SHIFT = 0x00000008;
    // Bits in the action code that represent a pointer index, used with ACTION_POINTER_DOWN and ACTION_POINTER_UP.
    static const int32_t ACTION_POINTER_INDEX_MASK = 0x0000ff00;
    // Bit shift for the action bits holding the pointer index as defined by ACTION_POINTER_INDEX_MASK.
    static const int32_t ACTION_POINTER_INDEX_SHIFT = 0x00000008;
    // Constant for getActionMasked(): A non-primary pointer has gone up.
    static const int32_t ACTION_POINTER_UP = 0x00000006;
    // Constant for getActionMasked(): The motion event contains relative vertical and/or horizontal scroll offsets.
    static const int32_t ACTION_SCROLL = 0x00000008;
    // Constant for getActionMasked(): A pressed gesture has finished, the motion contains the final release location as well as any intermediate points since the last down or move event.
    static const int32_t ACTION_UP = 0x00000001;
    // Axis constant: Brake axis of a motion event.
    static const int32_t AXIS_BRAKE = 0x00000017;
    // Axis constant: Distance axis of a motion event.
    static const int32_t AXIS_DISTANCE = 0x00000018;
    // Axis constant: Gas axis of a motion event.
    static const int32_t AXIS_GAS = 0x00000016;
    // Axis constant: Generic 1 axis of a motion event.
    static const int32_t AXIS_GENERIC_1 = 0x00000020;
    // Axis constant: Generic 10 axis of a motion event.
    static const int32_t AXIS_GENERIC_10 = 0x00000029;
    // Axis constant: Generic 11 axis of a motion event.
    static const int32_t AXIS_GENERIC_11 = 0x0000002a;
    // Axis constant: Generic 12 axis of a motion event.
    static const int32_t AXIS_GENERIC_12 = 0x0000002b;
    // Axis constant: Generic 13 axis of a motion event.
    static const int32_t AXIS_GENERIC_13 = 0x0000002c;
    // Axis constant: Generic 14 axis of a motion event.
    static const int32_t AXIS_GENERIC_14 = 0x0000002d;
    // Axis constant: Generic 15 axis of a motion event.
    static const int32_t AXIS_GENERIC_15 = 0x0000002e;
    // Axis constant: Generic 16 axis of a motion event.
    static const int32_t AXIS_GENERIC_16 = 0x0000002f;
    // Axis constant: Generic 2 axis of a motion event.
    static const int32_t AXIS_GENERIC_2 = 0x00000021;
    // Axis constant: Generic 3 axis of a motion event.
    static const int32_t AXIS_GENERIC_3 = 0x00000022;
    // Axis constant: Generic 4 axis of a motion event.
    static const int32_t AXIS_GENERIC_4 = 0x00000023;
    // Axis constant: Generic 5 axis of a motion event.
    static const int32_t AXIS_GENERIC_5 = 0x00000024;
    // Axis constant: Generic 6 axis of a motion event.
    static const int32_t AXIS_GENERIC_6 = 0x00000025;
    // Axis constant: Generic 7 axis of a motion event.
    static const int32_t AXIS_GENERIC_7 = 0x00000026;
    // Axis constant: Generic 8 axis of a motion event.
    static const int32_t AXIS_GENERIC_8 = 0x00000027;
    // Axis constant: Generic 9 axis of a motion event.
    static const int32_t AXIS_GENERIC_9 = 0x00000028;
    // Axis constant: Hat X axis of a motion event.
    static const int32_t AXIS_HAT_X = 0x0000000f;
    // Axis constant: Hat Y axis of a motion event.
    static const int32_t AXIS_HAT_Y = 0x00000010;
    // Axis constant: Horizontal Scroll axis of a motion event.
    static const int32_t AXIS_HSCROLL = 0x0000000a;
    // Axis constant: Left Trigger axis of a motion event.
    static const int32_t AXIS_LTRIGGER = 0x00000011;
    // Axis constant: Orientation axis of a motion event.
    static const int32_t AXIS_ORIENTATION = 0x00000008;
    // Axis constant: Pressure axis of a motion event.
    static const int32_t AXIS_PRESSURE = 0x00000002;
    // Axis constant: The movement of x position of a motion event.
    static const int32_t AXIS_RELATIVE_X = 0x0000001b;
    // Axis constant: The movement of y position of a motion event.
    static const int32_t AXIS_RELATIVE_Y = 0x0000001c;
    // Axis constant: Right Trigger axis of a motion event.
    static const int32_t AXIS_RTRIGGER = 0x00000012;
    // Axis constant: Rudder axis of a motion event.
    static const int32_t AXIS_RUDDER = 0x00000014;
    // Axis constant: X Rotation axis of a motion event.
    static const int32_t AXIS_RX = 0x0000000c;
    // Axis constant: Y Rotation axis of a motion event.
    static const int32_t AXIS_RY = 0x0000000d;
    // Axis constant: Z Rotation axis of a motion event.
    static const int32_t AXIS_RZ = 0x0000000e;
    // Axis constant: Size axis of a motion event.
    static const int32_t AXIS_SIZE = 0x00000003;
    // Axis constant: Throttle axis of a motion event.
    static const int32_t AXIS_THROTTLE = 0x00000013;
    // Axis constant: Tilt axis of a motion event.
    static const int32_t AXIS_TILT = 0x00000019;
    // Axis constant: ToolMajor axis of a motion event.
    static const int32_t AXIS_TOOL_MAJOR = 0x00000006;
    // Axis constant: ToolMinor axis of a motion event.
    static const int32_t AXIS_TOOL_MINOR = 0x00000007;
    // Axis constant: TouchMajor axis of a motion event.
    static const int32_t AXIS_TOUCH_MAJOR = 0x00000004;
    // Axis constant: TouchMinor axis of a motion event.
    static const int32_t AXIS_TOUCH_MINOR = 0x00000005;
    // Axis constant: Vertical Scroll axis of a motion event.
    static const int32_t AXIS_VSCROLL = 0x00000009;
    // Axis constant: Wheel axis of a motion event.
    static const int32_t AXIS_WHEEL = 0x00000015;
    // Axis constant: X axis of a motion event.
    static const int32_t AXIS_X = 0x00000000;
    // Axis constant: Y axis of a motion event.
    static const int32_t AXIS_Y = 0x00000001;
    // Axis constant: Z axis of a motion event.
    static const int32_t AXIS_Z = 0x0000000b;
    // Button constant: Back button pressed (mouse back button).
    static const int32_t BUTTON_BACK = 0x00000008;
    // Button constant: Forward button pressed (mouse forward button).
    static const int32_t BUTTON_FORWARD = 0x00000010;
    // Button constant: Primary button (left mouse button).
    static const int32_t BUTTON_PRIMARY = 0x00000001;
    // Button constant: Secondary button (right mouse button).
    static const int32_t BUTTON_SECONDARY = 0x00000002;
    // Button constant: Primary stylus button pressed.
    static const int32_t BUTTON_STYLUS_PRIMARY = 0x00000020;
    // Button constant: Secondary stylus button pressed.
    static const int32_t BUTTON_STYLUS_SECONDARY = 0x00000040;
    // Button constant: Tertiary button (middle mouse button).
    static const int32_t BUTTON_TERTIARY = 0x00000004;
    // Flag indicating the motion event intersected the bottom edge of the screen.
    static const int32_t EDGE_BOTTOM = 0x00000002;
    // Flag indicating the motion event intersected the left edge of the screen.
    static const int32_t EDGE_LEFT = 0x00000004;
    // Flag indicating the motion event intersected the right edge of the screen.
    static const int32_t EDGE_RIGHT = 0x00000008;
    // Flag indicating the motion event intersected the top edge of the screen.
    static const int32_t EDGE_TOP = 0x00000001;
    // This flag indicates that the window that received this motion event is partly or wholly obscured by another visible window above it.
    static const int32_t FLAG_WINDOW_IS_OBSCURED = 0x00000001;
    // An invalid pointer id.
    static const int32_t INVALID_POINTER_ID = 0xffffffff;
    // Tool type constant: The tool is an eraser or a stylus being used in an inverted posture.
    static const int32_t TOOL_TYPE_ERASER = 0x00000004;
    // Tool type constant: The tool is a finger.
    static const int32_t TOOL_TYPE_FINGER = 0x00000001;
    // Tool type constant: The tool is a mouse or trackpad.
    static const int32_t TOOL_TYPE_MOUSE = 0x00000003;
    // Tool type constant: The tool is a stylus.
    static const int32_t TOOL_TYPE_STYLUS = 0x00000002;
    // Tool type constant: Unknown tool type.
    static const int32_t TOOL_TYPE_UNKNOWN = 0x00000000;

    ANDROID_EXPORT MotionEvent(MotionEvent&& other);

    // Create a new MotionEvent, filling in a subset of the basic motion values.
    ANDROID_EXPORT static MotionEvent obtain(std::chrono::milliseconds downTime, std::chrono::milliseconds eventTime, int32_t action, float x, float y, int32_t metaState);
    // Create a new MotionEvent, filling in all of the basic values that define the motion.
    ANDROID_EXPORT static MotionEvent obtain(std::chrono::milliseconds downTime, std::chrono::milliseconds eventTime, int32_t action, float x, float y, float pressure, float size, int32_t metaState, float xPrecision, float yPrecision, int32_t deviceId, int32_t edgeFlags);
    // Create a new MotionEvent, copying from an existing one.
    ANDROID_EXPORT static MotionEvent obtain(const MotionEvent& other);

    // Returns a string that represents the symbolic name of the specified unmasked action such as "ACTION_DOWN", "ACTION_POINTER_DOWN(3)" or an equivalent numeric constant such as "35" if unknown.
    ANDROID_EXPORT static String actionToString(int32_t action);
    // Gets an axis by its symbolic name such as "AXIS_X" or an equivalent numeric constant such as "42".
    ANDROID_EXPORT static int32_t axisFromString(String symbolicName);
    // Returns a string that represents the symbolic name of the specified axis such as "AXIS_X" or an equivalent numeric constant such as "42" if unknown.
    ANDROID_EXPORT static String axisToString(int32_t axis);

    // Return the kind of action being performed.
    ANDROID_EXPORT int32_t getAction();
    // Gets which button has been modified during a press or release action.
    ANDROID_EXPORT int32_t getActionButton();
    // Returns the value of the requested axis for the given pointer index (use getPointerId(int32_t) to find the pointer identifier for this index).
    ANDROID_EXPORT float getAxisValue(int32_t axis, int32_t pointerIndex);
    // getAxisValue(int32_t) for the first pointer index (may be an arbitrary pointer identifier).
    ANDROID_EXPORT float getAxisValue(int32_t axis);
    // Gets the state of all buttons that are pressed such as a mouse or stylus button.
    ANDROID_EXPORT int32_t getButtonState();
    // Gets the id for the device that this event came from.
    ANDROID_EXPORT int32_t getDeviceId();
    // Returns the time (in ms) when the user originally pressed down to start a stream of position events.
    ANDROID_EXPORT std::chrono::milliseconds getDownTime();
    // Retrieve the time this event occurred, in the uptimeMillis() time base.
    ANDROID_EXPORT std::chrono::milliseconds getEventTime();
    // Gets the motion event flags.
    ANDROID_EXPORT int32_t getFlags();
    // Returns the state of any meta / modifier keys that were in effect when the event was generated.
    ANDROID_EXPORT int32_t getMetaState();
    // getOrientation(int32_t) for the first pointer index (may be an arbitrary pointer identifier).
    ANDROID_EXPORT float getOrientation();
    // Returns the orientation of the touch area and tool area in radians clockwise from vertical for the given pointer index (use getPointerId(int32_t) to find the pointer identifier for this index).
    ANDROID_EXPORT float getOrientation(int32_t pointerIndex);
    // The number of pointers of data contained in this event.
    ANDROID_EXPORT int32_t getPointerCount();
    // Returns the current pressure of this event for the given pointer index (use getPointerId(int32_t) to find the pointer identifier for this index).
    ANDROID_EXPORT float getPressure(int32_t pointerIndex);
    // getPressure(int32_t) for the first pointer index (may be an arbitrary pointer identifier).
    ANDROID_EXPORT float getPressure();
    // Returns the original raw X coordinate of this event.
    ANDROID_EXPORT float getRawX();
    // Returns the original raw Y coordinate of this event.
    ANDROID_EXPORT float getRawY();
    // Returns a scaled value of the approximate size for the given pointer index (use getPointerId(int32_t) to find the pointer identifier for this index).
    ANDROID_EXPORT float getSize(int32_t pointerIndex);
    // getSize(int32_t) for the first pointer index (may be an arbitrary pointer identifier).
    ANDROID_EXPORT float getSize();
    // Gets the source of the event.
    ANDROID_EXPORT int32_t getSource();
    // Returns the X coordinate of this event for the given pointer index (use getPointerId(int32_t) to find the pointer identifier for this index).
    ANDROID_EXPORT float getX(int32_t pointerIndex);
    // getX(int32_t) for the first pointer index (may be an arbitrary pointer identifier).
    ANDROID_EXPORT float getX();
    // Return the precision of the X coordinates being reported.
    ANDROID_EXPORT float getXPrecision();
    // getY(int32_t) for the first pointer index (may be an arbitrary pointer identifier).
    ANDROID_EXPORT float getY();
    // Returns the Y coordinate of this event for the given pointer index (use getPointerId(int32_t) to find the pointer identifier for this index).
    ANDROID_EXPORT float getY(int32_t pointerIndex);
    // Return the precision of the Y coordinates being reported.
    ANDROID_EXPORT float getYPrecision();
    // Checks if a mouse or stylus button (or combination of buttons) is pressed.
    ANDROID_EXPORT boolean isButtonPressed(int32_t button);
    // Adjust this event's location.
    ANDROID_EXPORT void offsetLocation(float deltaX, float deltaY);
    // Recycle the MotionEvent, to be re-used by a later caller.
    ANDROID_EXPORT void recycle();
    // Sets this event's action.
    ANDROID_EXPORT void setAction(int32_t action);
    // Set this event's location.
    ANDROID_EXPORT void setLocation(float x, float y);
    // Modifies the source of the event.
    ANDROID_EXPORT void setSource(int32_t source);

private:
    std::chrono::milliseconds m_eventTime { 0 };
    std::chrono::milliseconds m_downTime { 0 };
    int32_t m_action { 0 };
    PointF m_location;
    PointF m_rawLocation;
    float m_pressure { 0.0f };
    float m_size { 0.0f };
    int32_t m_metaState { 0 };
    float m_xPrecision { 0.0f };
    float m_yPrecision { 0.0f };
    int32_t m_flags { 0 };
    int32_t m_source { 0 };

    std::unique_ptr<MotionEventPrivate> m_private;
};

} // namespace view
} // namespace android

using MotionEvent = android::view::MotionEvent;
