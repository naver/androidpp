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

#include <android/widget/PopupMenu.h>

#include <atlbase.h>
#include <atlapp.h>
#include <atlwin.h>
#include <atlcrack.h>
#include <atlctrls.h>
#include <atlmisc.h>

namespace android {
namespace widget {

// FIXME: Add PopMenuProvider, then change this class to PopupMenuProviderWin.
class PopupMenuWin : public PopupMenu, public CWindowImpl<PopupMenuWin, CListBox> {
    friend class PopupMenu;
public:
    ~PopupMenuWin();

    // TODO: It would be nicer if encapsulated Menu/MenuItem are provided.
    void add(String&) override;
    void setSelected(int32_t) override;

    void show(Rect&) override;
    void dismiss() override;

    HWND hwnd() const;
    HWND parentHWND() const;

    BEGIN_MSG_MAP(PopupMenuWin)
        MSG_WM_CHAR(OnChar)
        MSG_WM_KEYUP(OnKeyUp)
        MSG_WM_KILLFOCUS(OnKillFocus)
        MSG_WM_LBUTTONUP(OnLButtonUp)
        MSG_WM_LBUTTONDOWN(OnLButtonDown)
        MSG_WM_KEYUP(OnKeyUp)
        MSG_WM_MOUSEACTIVATE(OnMouseActivate)
        MSG_WM_MOUSEMOVE(OnMouseMove)
    END_MSG_MAP()

    void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
    void OnKillFocus(CWindow wndFocus);
    void OnLButtonUp(UINT nFlags, CPoint point);
    void OnLButtonDown(UINT nFlags, CPoint point);
    int OnMouseActivate(CWindow wndTopLevel, UINT nHitTest, UINT message);
    void OnMouseMove(UINT nFlags, CPoint point);
    void OnFinalMessage(HWND) override;

private:
    PopupMenuWin(View&);

    bool m_shown;
    bool m_buttonDown;
};

} // namespace widget
} // namespace android
