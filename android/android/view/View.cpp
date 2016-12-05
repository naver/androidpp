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

#include "View.h"

#include <android/view/ViewHostWindow.h>
#include <android/view/ViewPrivate.h>
#include <android/view/WindowProvider.h>

namespace android {
namespace view {

View::View()
    : m_inWindow(false)
    , m_visibility(INVISIBLE)
    , m_hasFocus(false)
    , m_focusable(false)
    , m_private(std::make_unique<ViewPrivate>(*this))
{
}

View::~View()
{
    if (hasFocus())
        m_private->releaseFocus();
}

void View::setVisibility(int32_t visibility)
{
    m_visibility = visibility;
}

void View::setFocusable(bool focusable)
{
    if (isFocusable() || focusable == m_focusable)
        return;

    bool focused = hasFocus();

    m_focusable = focusable;

    if (!m_focusable && focused)
        m_private->releaseFocus();
}

bool View::requestFocus()
{
    if (!isFocusable())
        return false;

    m_private->requestFocus();
    return true;
}

void View::invalidate()
{
}

void View::invalidate(Rect&)
{
}

void View::onAttachedToWindow()
{
    m_inWindow = true;
}

void View::onDetachedFromWindow()
{
    m_inWindow = false;
}

void View::onWindowVisibilityChanged(int32_t visibility)
{
}

void View::onFocusChanged(bool gainFocus, int32_t direction, Rect& previouslyFocusedRect)
{
    m_hasFocus = gainFocus;

    if (m_focusChangeListener)
        m_focusChangeListener(this, gainFocus);
}

void View::onLayout(Rect& rc)
{
    m_rect = rc;
}

void View::onMeasure(int32_t widthMeasureSpec, int32_t heightMeasureSpec)
{
}

} // namespace view
} // namespace android
