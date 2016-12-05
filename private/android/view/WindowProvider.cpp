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

#include "WindowProvider.h"

namespace android {
namespace view {

WindowProvider::WindowProvider(ViewHostWindow& viewHost)
    : m_viewHost(viewHost)
{
}

bool WindowProvider::createPopupWindow(const Rect& clientRect)
{
    return platformCreatePopupWindow(clientRect);
}

void WindowProvider::closePopupWindow()
{
    platformClosePopupWindow();
}

WindowHandle WindowProvider::windowHandle() const
{
    return platformWindowHandle();
}

void WindowProvider::startInputMethod(bool enable)
{
    platformStartInputMethod(enable);
}

void WindowProvider::resetInputMethod()
{
    platformResetInputMethod();
}

void WindowProvider::setCursor(const std::shared_ptr<Cursor>& cursor)
{
    platformSetCursor(cursor);
}

void WindowProvider::setWindowFocus(bool focus)
{
    platformSetWindowFocus(focus);
}

void WindowProvider::setWindowCaptureMouse(bool capture)
{
    platformSetWindowCaptureMouse(capture);
}

void WindowProvider::setWindowPosition(int32_t x, int32_t y, int32_t width, int32_t height)
{
    platformSetWindowPosition(x, y, width, height);
}

void WindowProvider::sendWindowMove()
{
    platformSendWindowMove();
}

float WindowProvider::deviceScaleFactor() const
{
    return platformDeviceScaleFactor();
}

} // namespace view
} // namespace android
