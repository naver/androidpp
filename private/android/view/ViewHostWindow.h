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

#include "WindowHandle.h"
#include <android/graphics/Point.h>
#include <android/graphics/Rect.h>
#include <android/view/KeyEvent.h>
#include <android/view/MotionEvent.h>
#include <android/view/appkit/Cursor.h>
#include <android/view/inputmethod/InputConnection.h>
#include <platforms/Functional.h>

namespace android {
namespace view {

class View;
class WindowProvider;

class ANDROID_EXPORT ViewHostWindow final {
public:
    static std::unique_ptr<ViewHostWindow> create(WindowHandle parentWindow, const Rect& clientRect);
    static ViewHostWindow* createPopup(const Rect& clientRect);
    ~ViewHostWindow();

    enum Resize {
        Restored,
        Minimized,
        Maximized,
    };

    typedef std::safe_function<void (bool)> OnWindowPositionChangeListener;

    void windowCreated();
    void windowDestroyed();

    void windowPositionChanged(int32_t, int32_t);
    void windowSizeChanged(int32_t, int32_t, Resize);
    void windowFocused(bool);
    void windowIsVisible(bool);

    void dispatchKeyEvent(KeyEvent&);
    void dispatchMouseEvent(MotionEvent&);

    // FIXME: Need to support multiple popup window creation.
    bool createPopupWindow(const Rect& clientRect);
    void closePopupWindow();

    WindowHandle windowHandle() const;

    void setContentView(std::shared_ptr<View>);

    void invalidate();
    void invalidate(Rect&);

    bool startInputMethod(View*, bool);
    void endInputMethod(View*);
    InputConnection* inputConnection() const { return m_inputConnection.get(); }

    void requestFocus(View*);
    void releaseFocus(View*);

    void setCursor(const std::shared_ptr<Cursor>&);

    void sendWindowMove();
    void setWindowPosition(int32_t x, int32_t y, int32_t width, int32_t height);
    void setOnWindowPositionChangeListener(OnWindowPositionChangeListener listener);

    float deviceScaleFactor() const;
    void dpiChanged(int dpi);

private:
    ViewHostWindow(WindowHandle parentWindow, const Rect& clientRect);

    View* decorView() const;

    void windowRestored();
    void windowIsGone();

    int32_t m_width;
    int32_t m_height;
    bool m_isVisible;
    bool m_isGone;
    bool m_isFocused;
    View* m_focusedView;
    std::unique_ptr<InputConnection> m_inputConnection;

    class DecorView;
    std::unique_ptr<DecorView> m_decorView;
    std::unique_ptr<WindowProvider> m_windowProvider;
    OnWindowPositionChangeListener m_windowPositionChangeListener;
};

} // namespace view
} // namespace android
