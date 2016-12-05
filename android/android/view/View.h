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

#include <android/content/Context.h>
#include <android/content/res/Configuration.h>
#include <android/graphics/Point.h>
#include <android/graphics/Rect.h>
#include <android/os/IBinder.h>
#include <android/view/inputmethod/EditorInfo.h>
#include <android/view/inputmethod/InputConnection.h>

namespace android {
namespace view {

class KeyEvent;
class MotionEvent;
class ViewGroup;
class ViewHostWindow;
class ViewPrivate;

class View {
    friend class ViewGroup;
    friend class ViewHostWindow;
    friend class ViewPrivate;
public:
    // This view is visible.
    static const int32_t VISIBLE = 0;
    // This view is invisible, but it still takes up space for layout purposes.
    static const int32_t INVISIBLE = 4;
    // This view is invisible, and it doesn't take any space for layout purposes.
    static const int32_t GONE = 8;

    static const int32_t FOCUS_BACKWARD = 1;
    static const int32_t FOCUS_FORWARD = 2;
    static const int32_t FOCUS_LEFT = 17;
    static const int32_t FOCUS_RIGHT = 66;
    static const int32_t FOCUS_UP = 33;
    static const int32_t FOCUS_DOWN = 130;

    ANDROID_EXPORT View();
    ANDROID_EXPORT virtual ~View();

    typedef std::function<void (View*)> OnClickListener;
    typedef std::function<void (View*, bool)> OnFocusChangeListener;

    // Returns the context the view is running in, through which it can access the current theme, resources, etc.
    ANDROID_EXPORT Context& getContext();
    // Retrieve a unique token identifying the window this view is attached to.
    ANDROID_EXPORT IBinder getWindowToken();

    // Return the width of the your view.
    ANDROID_EXPORT int32_t getWidth() { return m_rect.width(); }
    // Return the height of your view.
    ANDROID_EXPORT int32_t getHeight() { return m_rect.height(); }

    // Returns the visibility status for this view.
    ANDROID_EXPORT virtual int32_t getVisibility() { return m_visibility; }
    // Set the visibility state of this view.
    ANDROID_EXPORT virtual void setVisibility(int32_t);

    // Returns true if this view is currently attached to a window.
    ANDROID_EXPORT bool isAttachedToWindow() { return m_inWindow; }
    // Find the view in the hierarchy rooted at this view that currently has focus.
    ANDROID_EXPORT virtual View* findFocus() { return (hasFocus()) ? this : nullptr; }
    // Returns true if this view has focus itself, or is the ancestor of the view that has focus.
    ANDROID_EXPORT bool hasFocus() { return m_hasFocus; }
    // Returns whether this View is able to take focus.
    ANDROID_EXPORT bool isFocusable() { return m_focusable; }
    // Set whether this view can receive the focus.
    ANDROID_EXPORT virtual void setFocusable(bool);
    // Call this to try to give focus to a specific view or to one of its descendants.
    ANDROID_EXPORT bool requestFocus();

    // Invalidate the whole view.
    ANDROID_EXPORT virtual void invalidate();
    // Mark the area defined by dirty as needing to be drawn.
    ANDROID_EXPORT virtual void invalidate(Rect&);

    // Return whether this view has an attached OnClickListener.
    ANDROID_EXPORT virtual bool hasOnClickListeners() { return m_clickListener.target<void (View*)>() != nullptr; }
    // Register a callback to be invoked when this view is clicked.
    ANDROID_EXPORT virtual void setOnClickListener(OnClickListener listener) { m_clickListener = listener; }
    // Directly call any attached OnClickListener.
    ANDROID_EXPORT virtual bool callOnClick() { if (!hasOnClickListeners()) return false; m_clickListener(this); return true; }

    // Returns the focus-change callback registered for this view.
    ANDROID_EXPORT virtual OnFocusChangeListener getOnFocusChangeListener() { return m_focusChangeListener; }
    // Register a callback to be invoked when focus of this view changed.
    ANDROID_EXPORT virtual void setOnFocusChangeListener(OnFocusChangeListener listener) { m_focusChangeListener = listener; }

    // Create a new InputConnection for an InputMethod to interact with the view.
    ANDROID_EXPORT virtual std::unique_ptr<InputConnection> onCreateInputConnection(EditorInfo& outAttrs);
    // Implement this method to handle generic motion events.
    ANDROID_EXPORT virtual bool onGenericMotionEvent(MotionEvent& event);
    // Default implementation of KeyEvent.Callback.onKeyDown(): perform press of the view when KEYCODE_DPAD_CENTER or KEYCODE_ENTER is released, if the view is enabled and clickable.
    ANDROID_EXPORT virtual bool onKeyDown(int32_t keyCode, KeyEvent& event);
    // Default implementation of KeyEvent.Callback.onKeyLongPress(): always returns false (doesn't handle the event).
    ANDROID_EXPORT virtual bool onKeyLongPress(int32_t keyCode, KeyEvent& event);
    // Default implementation of KeyEvent.Callback.onKeyMultiple(): always returns false (doesn't handle the event).
    ANDROID_EXPORT virtual bool onKeyMultiple(int32_t keyCode, int32_t repeatCount, KeyEvent& event);
    // Handle a key event before it is processed by any input method associated with the view hierarchy.
    ANDROID_EXPORT virtual bool onKeyPreIme(int32_t keyCode, KeyEvent& event);
    // Called on the focused view when a key shortcut event is not handled.
    ANDROID_EXPORT virtual bool onKeyShortcut(int32_t keyCode, KeyEvent& event);
    // Default implementation of KeyEvent.Callback.onKeyUp(): perform clicking of the view when KEYCODE_DPAD_CENTER, KEYCODE_ENTER or KEYCODE_SPACE is released. 
    ANDROID_EXPORT virtual bool onKeyUp(int32_t keyCode, KeyEvent& event);
    // Called when the window containing this view gains or loses focus.
    ANDROID_EXPORT virtual void onWindowFocusChanged(bool hasWindowFocus);

protected:
    // This is called when the view is attached to a window.
    ANDROID_EXPORT virtual void onAttachedToWindow();
    // This is called when the view is detached from a window.
    ANDROID_EXPORT virtual void onDetachedFromWindow();
    // Called when the window containing has change its visibility (between GONE, INVISIBLE, and VISIBLE).
    ANDROID_EXPORT virtual void onWindowVisibilityChanged(int32_t visibility);
    // Called by the view system when the focus state of this view changes.
    ANDROID_EXPORT virtual void onFocusChanged(bool gainFocus, int32_t direction, Rect& previouslyFocusedRect);
    // Called from layout when this view should assign a size and position to each of its children. 
    ANDROID_EXPORT virtual void onLayout(Rect&);
    // Measure the view and its content to determine the measured width and the measured height.
    ANDROID_EXPORT virtual void onMeasure(int32_t, int32_t);
    // Called when the current configuration of the resources being used by the application have changed.
    ANDROID_EXPORT virtual void onConfigurationChanged(Configuration&) { }

private:
    bool m_inWindow;
    Rect m_rect;
    int32_t m_visibility;
    bool m_hasFocus;
    bool m_focusable;

    OnClickListener m_clickListener;
    OnFocusChangeListener m_focusChangeListener;

    std::unique_ptr<ViewPrivate> m_private;
};

} // namespace view
} // namespace android

using View = android::view::View;
