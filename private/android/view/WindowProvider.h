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
#include <android/graphics/Rect.h>
#include <android/view/appkit/Cursor.h>

namespace android {
namespace view {

class ViewHostWindow;

class ANDROID_EXPORT WindowProvider {
public:
    static std::unique_ptr<WindowProvider> create(WindowHandle parentWindow, const Rect& clientRect,
        ViewHostWindow& viewHost);
    virtual ~WindowProvider() { }

    // FIXME: Need to support multiple popup window creation.
    bool createPopupWindow(const Rect& clientRect);
    void closePopupWindow();

    WindowHandle windowHandle() const;

    void startInputMethod(bool);
    void resetInputMethod();
    void setCursor(const std::shared_ptr<Cursor>&);
    void setWindowFocus(bool);
    void setWindowCaptureMouse(bool);
    void setWindowPosition(int32_t x, int32_t y, int32_t width, int32_t height);
    void sendWindowMove();
    float deviceScaleFactor() const;

protected:
    WindowProvider(ViewHostWindow&);

    virtual bool platformCreatePopupWindow(const Rect& clientRect) = 0;
    virtual void platformClosePopupWindow() = 0;

    virtual WindowHandle platformWindowHandle() const = 0;

    virtual void platformStartInputMethod(bool) = 0;
    virtual void platformResetInputMethod() = 0;
    virtual void platformSetCursor(const std::shared_ptr<Cursor>&) = 0;
    virtual void platformSetWindowFocus(bool) = 0;
    virtual void platformSetWindowCaptureMouse(bool) = 0;
    virtual void platformSetWindowPosition(int32_t x, int32_t y, int32_t width, int32_t height) = 0;
    virtual void platformSendWindowMove() = 0;
    virtual float platformDeviceScaleFactor() const = 0;

    ViewHostWindow& m_viewHost;
};

} // namespace view
} // namespace android
