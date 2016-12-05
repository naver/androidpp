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

#include "WindowProvider.h"

#include <android/graphics/PointF.h>
#include <android/view/appkit/VirtualKeyMap.h>
#include <android/view/inputmethod/CursorAnchorInfo.h>

#include <atlbase.h>
#include <atlwin.h>
#include <atlapp.h>
#include <atlcrack.h>
#include <atlmisc.h>

#include <vector>
#include <map>

namespace android {
namespace view {

#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0
#endif

class WindowProviderWin : public WindowProvider, public CWindowImpl<WindowProviderWin> {
    friend class WindowProvider;
public:
    DECLARE_WND_CLASS_EX(L"WTL_WindowProviderWin", CS_DBLCLKS, 0);
    virtual ~WindowProviderWin();

private:
    WindowProviderWin(WindowHandle parentWindow, const Rect& clientRect, ViewHostWindow&);

    bool platformCreatePopupWindow(const Rect& clientRect) override;
    void platformClosePopupWindow() override;

    WindowHandle platformWindowHandle() const override;

    void platformStartInputMethod(bool) override;
    void platformResetInputMethod() override;
    void platformSetCursor(const std::shared_ptr<Cursor>&) override;
    void platformSetWindowFocus(bool) override;
    void platformSetWindowCaptureMouse(bool) override;
    void platformSetWindowPosition(int32_t x, int32_t y, int32_t width, int32_t height) override;
    void platformSendWindowMove() override;
    float platformDeviceScaleFactor() const override;

    HWND createPopupWindow(const Rect& clientRect);
    static LRESULT CALLBACK popupWindowWndProc(HWND, UINT, WPARAM, LPARAM);
    LRESULT popupWindowEventHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    HWND hwnd() const;

    PointF toScreen(const CPoint& point);
    PointF fromScreen(const CPoint& point);

    BEGIN_MSG_MAP_EX(WindowProviderWin)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_DESTROY(OnDestroy)
        MSG_WM_SETFOCUS(OnSetFocus)
        MSG_WM_KILLFOCUS(OnKillFocus)
        MSG_WM_SETCURSOR(OnSetCursor)
        MSG_WM_SHOWWINDOW(OnShowWindow)
        MSG_WM_PAINT(OnPaint)
        MSG_WM_ERASEBKGND(OnEraseBkgnd)
        MSG_WM_MOVE(OnMove)
        MSG_WM_SIZE(OnSize)
        MSG_WM_LBUTTONDOWN(OnLButtonDown)
        MSG_WM_LBUTTONDBLCLK(OnLButtonDoubleClick)
        MSG_WM_LBUTTONUP(OnLButtonUp)
        MSG_WM_MBUTTONDOWN(OnMButtonDown)
        MSG_WM_MBUTTONDBLCLK(OnMButtonDoubleClick)
        MSG_WM_MBUTTONUP(OnMButtonUp)
        MSG_WM_RBUTTONDOWN(OnRButtonDown)
        MSG_WM_RBUTTONDBLCLK(OnRButtonDoubleClick)
        MSG_WM_RBUTTONUP(OnRButtonUp)
        MSG_WM_MOUSEMOVE(OnMouseMove)
        MSG_WM_MOUSELEAVE(OnMouseLeave)
        MSG_WM_MOUSEWHEEL(OnMouseWheel)
        MSG_WM_KEYDOWN(OnKeyDown)
        MSG_WM_SYSKEYDOWN(OnSysKeyDown)
        MSG_WM_KEYUP(OnKeyUp)
        MSG_WM_SYSKEYUP(OnSysKeyUp)
        MSG_WM_CHAR(OnChar)
        MSG_WM_SYSCHAR(OnSysChar)
        MESSAGE_HANDLER_EX(WM_DPICHANGED, OnDpiChanged)
        MESSAGE_HANDLER_EX(WM_IME_STARTCOMPOSITION, OnImeStartComposition)
        MESSAGE_HANDLER_EX(WM_IME_REQUEST, OnImeRequest)
        MESSAGE_HANDLER_EX(WM_IME_COMPOSITION, OnImeComposition)
        MESSAGE_HANDLER_EX(WM_IME_ENDCOMPOSITION, OnImeEndComposition)
        MESSAGE_HANDLER_EX(WM_IME_CHAR, OnImeChar)
        MESSAGE_HANDLER_EX(WM_IME_NOTIFY, OnImeNotify)
        MESSAGE_HANDLER_EX(WM_IME_SELECT, OnImeSelect)
        MESSAGE_HANDLER_EX(WM_IME_SETCONTEXT, OnImeSetContext)
    END_MSG_MAP()

    int OnCreate(LPCREATESTRUCT lpCreateStruct);
    void OnDestroy();

    void OnSetFocus(CWindow wndOld);
    void OnKillFocus(CWindow wndFocus);
    BOOL OnSetCursor(CWindow wnd, UINT nHitTest, UINT message);

    void OnShowWindow(BOOL bShow, UINT nStatus);
    void OnPaint(CDCHandle dc);
    BOOL OnEraseBkgnd(CDCHandle dc);
    void OnMove(CPoint position);
    void OnSize(UINT nType, CSize size);

    void OnLButtonDown(UINT nFlags, CPoint point);
    void OnLButtonDoubleClick(UINT nFlags, CPoint point);
    void OnLButtonUp(UINT nFlags, CPoint point);
    void OnMButtonDown(UINT nFlags, CPoint point);
    void OnMButtonDoubleClick(UINT nFlags, CPoint point);
    void OnMButtonUp(UINT nFlags, CPoint point);
    void OnRButtonDown(UINT nFlags, CPoint point);
    void OnRButtonDoubleClick(UINT nFlags, CPoint point);
    void OnRButtonUp(UINT nFlags, CPoint point);
    void OnMouseMove(UINT nFlags, CPoint point);
    void OnMouseLeave();
    BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint point);

    void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
    void OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
    void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    void OnSysChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    LRESULT OnDpiChanged(UINT uMsg, WPARAM wParam, LPARAM lParam);

    void OnFinalMessage(HWND) override;

    std::shared_ptr<Cursor> m_cursor;
    std::shared_ptr<Cursor> m_pendingCursor;
    bool m_mouseEntered;

    void sendGeneratedMouseEnterEventIfNeeded(UINT nFlags, CPoint point);

    std::chrono::milliseconds m_downTime[256];

    std::chrono::milliseconds generateDownTime(int32_t);
    std::chrono::milliseconds getDownTime(int32_t);

    LRESULT OnImeStartComposition(UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT OnImeRequest(UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT OnImeComposition(UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT OnImeEndComposition(UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT OnImeChar(UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT OnImeNotify(UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT OnImeSelect(UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT OnImeSetContext(UINT message, WPARAM wParam, LPARAM lParam);

    HMODULE m_imm32;
    UINT m_batchCount;

    typedef HIMC(CALLBACK *getContextPtr)(HWND);
    typedef BOOL(CALLBACK *releaseContextPtr)(HWND, HIMC);
    typedef LONG(CALLBACK *getCompositionStringPtr)(HIMC, DWORD, LPVOID, DWORD);
    typedef BOOL(CALLBACK *setCandidateWindowPtr)(HIMC, LPCANDIDATEFORM);
    typedef BOOL(CALLBACK *setOpenStatusPtr)(HIMC, BOOL);
    typedef BOOL(CALLBACK *notifyIMEPtr)(HIMC, DWORD, DWORD, DWORD);
    typedef BOOL(CALLBACK *associateContextExPtr)(HWND, HIMC, DWORD);

    getContextPtr immGetContext;
    releaseContextPtr immReleaseContext;
    getCompositionStringPtr immGetCompositionString;
    setCandidateWindowPtr immSetCandidateWindow;
    setOpenStatusPtr immSetOpenStatus;
    notifyIMEPtr immNotifyIME;
    associateContextExPtr immAssociateContextEx;

    void loadIMM();
    HIMC getIMMContext();
    void releaseIMMContext(HIMC);
    void setCandidateWindow(HIMC);
    void resetIME();
    void enableInputMethod(bool);
    void updateCursorAnchorInfo(const CursorAnchorInfo&);
    bool getCompositionString(HIMC, DWORD, String&);
    LRESULT onIMERequestCharPosition(IMECHARPOSITION*);
    LRESULT onIMERequestReconvertString(RECONVERTSTRING*);

    bool m_isCompositing;
    bool m_requestedCursorUpdates;
    CursorAnchorInfo m_cursorAnchorInfo;

    static std::map<WindowProviderWin*, std::shared_ptr<ViewHostWindow>> m_popupWindowHolder;
};

} // namespace view
} // namespace android
