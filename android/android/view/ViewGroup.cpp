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

#include "ViewGroup.h"

#include <android/view/ViewPrivate.h>
#include <android/view/WindowProvider.h>
#include <android/view/appkit/UIEvent.h>
#include <platforms/Functional.h>

namespace android {
namespace view {

ViewGroup::ViewGroup()
{
}

ViewGroup::~ViewGroup()
{
}

void ViewGroup::addView(std::shared_ptr<View> view)
{
    ViewPrivate& viewPrivate = view::getPrivate(*view);
    if (!view || viewPrivate.parentView())
        return;

    m_children.insert(m_children.begin(), view);
    viewPrivate.childViewAttached(view.get());

    if (!isAttachedToWindow())
        return;

    view->onAttachedToWindow();
}

void ViewGroup::bringChildToFront(std::shared_ptr<View>)
{
}

View* ViewGroup::findFocus()
{
    View* view = View::findFocus();
    if (!view)
        return propagate(m_children, &View::findFocus);

    return view;
}

View* ViewGroup::getChildAt(int index)
{
    if (m_children.size() <= index)
        return this;

    return m_children[index].get();
}

void ViewGroup::setVisibility(int32_t visibility)
{
    View::setVisibility(visibility);
    propagate(m_children, &View::setVisibility, visibility);
}

void ViewGroup::onAttachedToWindow()
{
    View::onAttachedToWindow();
    propagate(m_children, &View::onAttachedToWindow);
}

void ViewGroup::onDetachedFromWindow()
{
    View::onDetachedFromWindow();
    propagate(m_children, &View::onDetachedFromWindow);
}

void ViewGroup::onWindowVisibilityChanged(int visibility)
{
    View::onWindowVisibilityChanged(visibility);
    propagate(m_children, &View::onWindowVisibilityChanged, visibility);
}

void ViewGroup::onMeasure(int32_t widthMeasureSpec, int32_t heightMeasureSpec)
{
    View::onMeasure(widthMeasureSpec, heightMeasureSpec);
    propagate(m_children, &View::onMeasure, widthMeasureSpec, heightMeasureSpec);
}

void ViewGroup::onLayout(Rect& rc)
{
    View::onLayout(rc);
    propagate(m_children, &View::onLayout, std::ref(rc));
}

void ViewGroup::onConfigurationChanged(Configuration& config)
{
    propagate(m_children, &View::onConfigurationChanged, config);
}

//bool ViewGroup::sendGenericEvent(UIEvent& event) // FIXME!
//{
//    if (!propagate(m_children, &View::sendGenericEvent, *(&event)))
//        return View::sendGenericEvent(event);
//
//    return true;
//}

} // namespace view
} // namespace android
