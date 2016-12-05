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

#include "PopupMenuWin.h"

#include <android/view/View.h>
#include <android/view/ViewHostWindow.h>
#include <android/view/ViewPrivate.h>

#include <platforms/StringConversion.h>
#include <algorithm>

namespace android {
namespace widget {

static const int32_t popupMenuBorderHeight = 1;
static const int32_t maxPopupHeight = 320;

static const UINT WM_HOST_WINDOW_FIRST = WM_USER;
static const UINT WM_HOST_WINDOW_MOUSEMOVE = WM_USER + WM_MOUSEMOVE;

static void translatePoint(LPARAM& lParam, HWND from, HWND to)
{
    POINT pt;
    pt.x = static_cast<short>(GET_X_LPARAM(lParam));
    pt.y = static_cast<short>(GET_Y_LPARAM(lParam));
    ::MapWindowPoints(from, to, &pt, 1);
    lParam = MAKELPARAM(pt.x, pt.y);
}

std::unique_ptr<PopupMenu> PopupMenu::create(View& anchor)
{
    return std::unique_ptr<PopupMenu>(new PopupMenuWin(anchor));
}

PopupMenuWin::PopupMenuWin(View& anchor)
    : PopupMenu(anchor)
    , m_shown(false)
    , m_buttonDown(false)
{
    CWindowImpl<PopupMenuWin, CListBox>::Create(parentHWND(), rcDefault, NULL, WS_POPUP | WS_VSCROLL | WS_BORDER | LBS_NOINTEGRALHEIGHT,
        WS_EX_LTRREADING | WS_EX_NOACTIVATE);

    if (IsWindow())
        SetFont(AtlGetStockFont(DEFAULT_GUI_FONT));
}

PopupMenuWin::~PopupMenuWin()
{
    if (IsWindow()) {
        dismiss();
        DestroyWindow();
    }
}

void PopupMenuWin::add(String& text)
{
    AddString(text.c_str());
}

void PopupMenuWin::setSelected(int32_t index)
{
    SetCurSel(index);
}

void PopupMenuWin::show(Rect& anchorPosition)
{
    if (m_shown)
        return;

    CLogFont ft(AtlGetStockFont(DEFAULT_GUI_FONT));
    // FIXME : calcurate popupmenu`s height heuristically. It needs to correct this.
    int32_t naturalHeight = (-ft.lfHeight + popupMenuBorderHeight) * GetCount();
    int32_t popupHeight = std::min(maxPopupHeight, naturalHeight);

    POINT pt;
    RECT screenSize, windowSize;
    ::GetWindowRect(GetDesktopWindow(), &screenSize);
    ::GetWindowRect(parentHWND(), &windowSize);
    pt.x = anchorPosition.left;
    pt.y = windowSize.top + anchorPosition.bottom + popupHeight > screenSize.bottom ?
            anchorPosition.top - popupHeight : anchorPosition.bottom;

    ::ClientToScreen(parentHWND(), &pt);
    m_shown = SetWindowPos(NULL, pt.x, pt.y, anchorPosition.width(), popupHeight, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE);

    HWND hostWindow = parentHWND();

    MSG msg;
    HWND activeWindow;
    bool needToHandleInParent = false;

    while (::GetMessage(&msg, 0, 0, 0)) {
        switch (msg.message) {
        case WM_HOST_WINDOW_MOUSEMOVE:
             if (msg.hwnd == m_hWnd) {
                // This message should be sent to the host window.
                msg.hwnd = hostWindow;
                msg.message -= WM_HOST_WINDOW_FIRST;
             }
             break;

        // Steal mouse messages.
        case WM_NCMOUSEMOVE:
        case WM_NCLBUTTONDOWN:
        case WM_NCLBUTTONUP:
        case WM_NCLBUTTONDBLCLK:
        case WM_NCRBUTTONDOWN:
        case WM_NCRBUTTONUP:
        case WM_NCRBUTTONDBLCLK:
        case WM_NCMBUTTONDOWN:
        case WM_NCMBUTTONUP:
        case WM_NCMBUTTONDBLCLK:
        case WM_MOUSEWHEEL:
            msg.hwnd = hwnd();
            break;

        // These mouse messages use client coordinates so we need to convert them.
        case WM_LBUTTONUP: {
            if (!needToHandleInParent) {
                needToHandleInParent = true;
                break;
            }
            // fall through
        }
        case WM_MOUSEMOVE:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MBUTTONDBLCLK: {
            // Translate the coordinate.
            if (!needToHandleInParent)
                break;
            translatePoint(msg.lParam, msg.hwnd, hwnd());
            msg.hwnd = hwnd();
            break;
        }

        // Steal all keyboard messages.
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_CHAR:
        case WM_DEADCHAR:
        case WM_SYSKEYUP:
        case WM_SYSCHAR:
        case WM_SYSDEADCHAR:
            msg.hwnd = hwnd();
            break;
        }

        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);

        if (!m_shown)
            break;
        activeWindow = ::GetActiveWindow();
        if (activeWindow != hostWindow && !::IsChild(activeWindow, hostWindow))
            break;
    }

    dismiss();
}

void PopupMenuWin::dismiss()
{
    if (!m_shown)
        return;

    m_shown = false;
    ShowWindow(SW_HIDE);
    m_dismissListener(*this);
}

HWND PopupMenuWin::hwnd() const
{
    return static_cast<HWND>(*this);
}

HWND PopupMenuWin::parentHWND() const
{
    return reinterpret_cast<HWND>(view::getPrivate(m_anchor).hostWindow()->windowHandle());
}

void PopupMenuWin::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (nChar == VK_DOWN || nChar == VK_UP)
        m_menuItemFocusListener(GetCurSel());
}

void PopupMenuWin::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    SetMsgHandled(FALSE);
    if (nChar == VK_RETURN) {
        m_menuItemClickListener(GetCurSel());
        m_shown = false;
        ShowWindow(SW_HIDE);
    }
}

void PopupMenuWin::OnKillFocus(CWindow wndFocus)
{
    dismiss();
}

void PopupMenuWin::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (!m_buttonDown)
        return;

    m_buttonDown = false;
    RECT bounds;
    ::GetClientRect(hwnd(), &bounds);
    if (::PtInRect(&bounds, point))
        m_menuItemClickListener(GetCurSel());
    else
        m_dismissListener(*this);
    m_shown = false;
    ShowWindow(SW_HIDE);
}

void PopupMenuWin::OnLButtonDown(UINT nFlags, CPoint point)
{
    m_buttonDown = true;
}

int PopupMenuWin::OnMouseActivate(CWindow wndTopLevel, UINT nHitTest, UINT message)
{
    return MA_NOACTIVATE;
}

void PopupMenuWin::OnMouseMove(UINT nFlags, CPoint point)
{
    BOOL outside = TRUE;
    RECT bounds;
    ::GetClientRect(m_hWnd, &bounds);
    if (!::PtInRect(&bounds, point)) {
        LPARAM lParam = MAKELPARAM(point.x, point.y);
        translatePoint(lParam, m_hWnd, parentHWND());
        ::PostMessage(m_hWnd, WM_HOST_WINDOW_MOUSEMOVE, nFlags, lParam);
        return;
    }

    int hoveredIndex = static_cast<CListBox>(m_hWnd).ItemFromPoint(point, outside);
    if (outside || hoveredIndex < 0)
        return;

    SetCurSel(hoveredIndex);
}

void PopupMenuWin::OnFinalMessage(HWND)
{
}

} // namespace widget
} // namespace android
