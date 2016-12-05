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

#include <android/view/View.h>

namespace android {
namespace view {

class View;
class ViewHostWindow;

class ViewPrivate {
    friend class View;
    friend class ViewHostWindow;
public:
    ViewPrivate(View&);
    ~ViewPrivate();

    static ViewPrivate& getPrivate(View&);
    static void setPrivate(View&, std::unique_ptr<ViewPrivate>&&);

    virtual ViewHostWindow* hostWindow() const;

    View* thisView() const { return &m_thisView; }
    View* parentView() const { return m_parentView; }
    void setParentView(View* v) { m_parentView = v; }
    void childViewAttached(View* v) { v->m_private->setParentView(thisView()); }
    void childViewDetached(View* v) { if (v->m_private->m_parentView != thisView()) return; v->m_private->setParentView(nullptr); }

    void requestFocus();
    void releaseFocus();

private:
    View& m_thisView;
    View* m_parentView;
};

inline ViewPrivate& getPrivate(View& view)
{
    return ViewPrivate::getPrivate(view);
}

} // namespace view
} // namespace android
