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

#include "ViewHostWindow.h"

#include <android/view/InputDevice.h>
#include <android/view/View.h>
#include <android/view/ViewPrivate.h>
#include <android/view/WindowProvider.h>
#include <android/view/appkit/EditableContext.h>
#include <android/view/appkit/UIMouseEvent.h>

#include <algorithm>
#include <assert>

namespace android {
namespace view {

class DecorViewPrivate : public ViewPrivate {
public:
    DecorViewPrivate(View& view, ViewHostWindow& window)
        : ViewPrivate(view), m_window(window) { }
    ~DecorViewPrivate() = default;

    ViewHostWindow* hostWindow() const override { return &m_window; }

private:
    ViewHostWindow& m_window;
};

class ViewHostWindow::DecorView : public View {
public:
    DecorView(ViewHostWindow& window) { ViewPrivate::setPrivate(*this, std::make_unique<DecorViewPrivate>(*this, window)); }
    ~DecorView() = default;

    View* view() const { return m_view.get(); }
    void setView(std::shared_ptr<View> view);

private:
    std::shared_ptr<View> m_view;
};

void ViewHostWindow::DecorView::setView(std::shared_ptr<View> view)
{
    if (m_view)
        getPrivate(*this).childViewDetached(m_view.get());

    m_view = view;

    if (m_view)
        getPrivate(*this).childViewAttached(m_view.get());
}

std::unique_ptr<ViewHostWindow> ViewHostWindow::create(WindowHandle parentWindow, const Rect& clientRect)
{
    return std::unique_ptr<ViewHostWindow>(new ViewHostWindow(parentWindow, clientRect));
}

ViewHostWindow* ViewHostWindow::createPopup(const Rect& clientRect)
{
    // We will automatically manage ViewHostWindow instance in createPopupWindow().
    ViewHostWindow* hostWindow = new ViewHostWindow(WindowHandle(nullptr), clientRect);
    if (!hostWindow->createPopupWindow(clientRect)) {
        delete hostWindow;
        return nullptr;
    }

    return hostWindow;
}

ViewHostWindow::ViewHostWindow(WindowHandle parentWindow, const Rect& clientRect)
    : m_width(0)
    , m_height(0)
    , m_isVisible(false)
    , m_isGone(false)
    , m_isFocused(false)
    , m_focusedView(nullptr)
    , m_windowProvider(WindowProvider::create(parentWindow, clientRect, *this))
    , m_decorView(std::make_unique<DecorView>(*this))
{
}

ViewHostWindow::~ViewHostWindow()
{
}

View* ViewHostWindow::decorView() const
{
    return m_decorView->view();
}

void ViewHostWindow::windowCreated()
{
    if (!decorView())
        return;

    decorView()->onAttachedToWindow();
}

void ViewHostWindow::windowDestroyed()
{
    if (!decorView())
        return;

    decorView()->onDetachedFromWindow();
}

void ViewHostWindow::windowPositionChanged(int32_t x, int32_t y)
{
    if (!decorView())
        return;

    //decorView()->onWindowPositionChanged(Point(x, y));
}

void ViewHostWindow::windowSizeChanged(int32_t widthMeasureSpec, int32_t heightMeasureSpec, Resize resize)
{
    if (m_width == widthMeasureSpec && m_height == heightMeasureSpec)
        return;

    m_width = widthMeasureSpec;
    m_height = heightMeasureSpec;

    bool windowMinimized = widthMeasureSpec == 0 && heightMeasureSpec == 0;
    if (windowMinimized || resize == Minimized) {
        windowIsGone();
        return;
    } else if (m_isGone) {
        windowRestored();
    }
    
    if (!decorView())
        return;
    
    bool changed = decorView()->getWidth() != widthMeasureSpec || decorView()->getHeight() != heightMeasureSpec;
    if (!changed)
        return;
    
    decorView()->onMeasure(widthMeasureSpec, heightMeasureSpec);
    decorView()->onLayout(Rect(0, 0, widthMeasureSpec, heightMeasureSpec));
    invalidate();
}

void ViewHostWindow::windowFocused(bool focus)
{
    if (focus == m_isFocused)
        return;

    m_isFocused = focus;

    if (!decorView())
        return;
    
    decorView()->onWindowFocusChanged(focus);
}

void ViewHostWindow::windowIsVisible(bool visible)
{
    if (visible == m_isVisible)
        return;

    m_isVisible = visible;

    if (!m_isVisible) {
        if (decorView())
            decorView()->setVisibility(View::INVISIBLE);
        return;
    } else if (!m_isGone) {
        if (decorView())
            decorView()->setVisibility(View::VISIBLE);
    }
}

void ViewHostWindow::dispatchKeyEvent(KeyEvent& event)
{
    if (!decorView())
        return;

    View* eventTarget = decorView();
    switch (event.getAction()) {
    case KeyEvent::ACTION_DOWN:
        if (eventTarget->onKeyDown(event.getKeyCode(), event))
            return;
        break;
    case KeyEvent::ACTION_UP:
        if (eventTarget->onKeyUp(event.getKeyCode(), event))
            return;
        break;
    default:
        return;
    }

    bool editableEvent = false;

    if (inputConnection())
        if (inputConnection()->sendKeyEvent(event))
            editableEvent = true;
}

void ViewHostWindow::dispatchMouseEvent(MotionEvent& event)
{
    assert(event.getSource() == InputDevice::SOURCE_MOUSE);

    if (!decorView())
        return;

    View* eventTarget = decorView();
    switch (event.getAction()) {
    case MotionEvent::ACTION_BUTTON_PRESS:
        if (!event.getActionButton() == MotionEvent::BUTTON_SECONDARY)
            m_windowProvider->setWindowFocus(true);
        m_windowProvider->setWindowCaptureMouse(true);
        eventTarget->onGenericMotionEvent(event);
        return;
    case MotionEvent::ACTION_BUTTON_RELEASE:
        eventTarget->onGenericMotionEvent(event);
        m_windowProvider->setWindowCaptureMouse(false);
        return;
    default:
        eventTarget->onGenericMotionEvent(event);
        return;
    }
}

bool ViewHostWindow::createPopupWindow(const Rect& clientRect)
{
    return m_windowProvider->createPopupWindow(clientRect);
}

void ViewHostWindow::closePopupWindow()
{
    m_windowProvider->closePopupWindow();
}

WindowHandle ViewHostWindow::windowHandle() const
{
    return m_windowProvider->windowHandle();
}

void ViewHostWindow::setContentView(std::shared_ptr<View> view)
{
    if (decorView())
        decorView()->onDetachedFromWindow();

    m_decorView->setView(view);

    if (decorView()) {
        decorView()->onAttachedToWindow();
        decorView()->setVisibility((m_isGone) ? View::GONE : (m_isVisible) ? View::VISIBLE : View::INVISIBLE);
        decorView()->onMeasure(m_width, m_height);
        decorView()->onLayout(Rect(0, 0, m_width, m_height));

        if (m_isFocused)
            decorView()->onWindowFocusChanged(true);

        invalidate();
    }
}

void ViewHostWindow::invalidate()
{
    if (!decorView())
        return;

    decorView()->invalidate();
}

void ViewHostWindow::invalidate(Rect& rect)
{
    if (!decorView())
        return;

    decorView()->invalidate(rect);
}

bool ViewHostWindow::startInputMethod(View* view, bool enable)
{
    assert(decorView());

    m_inputConnection = view->onCreateInputConnection(EditorInfo());
    m_windowProvider->startInputMethod(enable);
    return true;
}

void ViewHostWindow::endInputMethod(View*)
{
    m_windowProvider->resetInputMethod();
    m_inputConnection.reset();
    m_windowProvider->startInputMethod(false);
}

void ViewHostWindow::requestFocus(View* view)
{
    if (view == m_focusedView)
        return;

    if (m_focusedView)
        m_focusedView->onFocusChanged(false, 0, Rect());

    m_focusedView = view;

    if (m_focusedView)
        m_focusedView->onFocusChanged(true, 0, Rect());
}

void ViewHostWindow::releaseFocus(View* view)
{
    if (view != m_focusedView)
        return;

    if (m_focusedView)
        m_focusedView->onFocusChanged(false, 0, Rect());

    m_focusedView = nullptr;
}

void ViewHostWindow::windowRestored()
{
    assert(m_isGone);

    m_isGone = false;

    if (!decorView())
        return;
    
    decorView()->onWindowVisibilityChanged(View::VISIBLE);
    decorView()->onLayout(Rect(0, 0, m_width, m_height));
}

void ViewHostWindow::windowIsGone()
{
    assert(!m_isGone);

    m_isGone = true;

    if (!decorView())
        return;
    
    decorView()->onWindowVisibilityChanged(View::GONE);
}

void ViewHostWindow::setCursor(const std::shared_ptr<Cursor>& cursor)
{
    m_windowProvider->setCursor(cursor);
}

class WindowPositionChangeScope {
public:
    WindowPositionChangeScope(ViewHostWindow::OnWindowPositionChangeListener& listener)
        : m_listener(listener)
    {
        m_listener(true);
    }
    ~WindowPositionChangeScope()
    {
        m_listener(false);
    }

private:
    ViewHostWindow::OnWindowPositionChangeListener& m_listener;
};

void ViewHostWindow::sendWindowMove()
{
    m_windowProvider->sendWindowMove();
}

void ViewHostWindow::setWindowPosition(int32_t x, int32_t y, int32_t width, int32_t height)
{
    WindowPositionChangeScope scope(m_windowPositionChangeListener);
    m_windowProvider->setWindowPosition(x, y, width, height);
}

void ViewHostWindow::setOnWindowPositionChangeListener(OnWindowPositionChangeListener listener)
{
    m_windowPositionChangeListener = std::move(listener);
}

float ViewHostWindow::deviceScaleFactor() const
{
    return m_windowProvider->deviceScaleFactor();
}

void ViewHostWindow::dpiChanged(int dpi)
{
    Configuration config;
    config.densityDpi = dpi;
    decorView()->onConfigurationChanged(config);
}

} // namespace view
} // namespace android
