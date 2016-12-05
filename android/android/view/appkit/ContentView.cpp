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

#include "ContentView.h"

#include <android/content/Context.h>
#include <android/content/res/Configuration.h>
#include <android/content/res/Resources.h>
#include <android/view/ViewHostWindow.h>
#include <android/view/WindowProvider.h>
#include <android/view/appkit/EditableContext.h>
#include <android/view/appkit/RenderContext.h>
#include <android/view/appkit/RenderContextList.h>
#include <android/view/appkit/UIEvent.h>
#include <android/view/appkit/ViewContext.h>
#include <android/view/inputmethod/InputMethodManager.h>

#include <platforms/Functional.h>

#include <algorithm>

namespace android {
namespace view {
namespace appkit {

class ContentView::ViewObserver {
public:
    ViewObserver(ContentView&);
    ~ViewObserver();

private:
    ContentView& m_view;
    ViewContext* m_foregroundView;
};

ContentView::ViewObserver::ViewObserver(ContentView& view)
    : m_view(view)
    , m_foregroundView(m_view.foregroundView())
{
}

ContentView::ViewObserver::~ViewObserver()
{
    ViewContext* foregroundView = m_view.foregroundView();
    if (foregroundView == m_foregroundView)
        return;

    ViewContext* backgroundView = m_foregroundView;
    if (m_view.hasFocus()) {
        if (backgroundView)
            backgroundView->setFocus(false);
        if (foregroundView)
            foregroundView->setFocus(true);
    }

    m_view.m_motionDetector.reset();
    m_view.updateContentsRenderState();
}

ContentView::ContentView()
    : m_renderClient(nullptr)
    , m_motionDetector(*this)
    , m_handler(os::Handler::create())
{
    setFocusable(true);
}

ContentView::~ContentView()
{
}

void ContentView::addView(std::shared_ptr<ViewContext> view)
{
    ViewObserver observer(*this);

    m_contents.push_back(view);

    if (isAttachedToWindow()) {
        view->attached(*this);
        view->onLayout(getWidth(), getHeight());
    }
}

void ContentView::removeView(std::shared_ptr<ViewContext> view)
{
    ViewObserver observer(*this);

    auto it = std::find(m_contents.begin(), m_contents.end(), view);
    if (it == m_contents.end())
        return;

    if (isAttachedToWindow())
        view->onDetachedFromWindow();

    detachViewFromRenderClient(*it);

    m_contents.erase(it);
}

void ContentView::moveAfter(std::shared_ptr<ViewContext> view, std::shared_ptr<ViewContext> after)
{
    ViewObserver observer(*this);

    auto iterAfter = std::find(m_contents.begin(), m_contents.end(), after);
    if (iterAfter == m_contents.end())
        return;

    auto iterView = std::find(m_contents.begin(), m_contents.end(), view);
    if (iterView == m_contents.end() || iterAfter + 1 == iterView)
        return;

    std::swap(*(iterAfter + 1), *(iterView));
}

void ContentView::bringChildToFront(std::shared_ptr<ViewContext> view)
{
    ViewObserver observer(*this);

    auto it = std::find(m_contents.begin(), m_contents.end(), view);
    if (it == m_contents.begin() || it == m_contents.end())
        return;

    std::rotate(m_contents.begin(), it, it + 1);
}

void ContentView::sendToBack(std::shared_ptr<ViewContext> view)
{
    ViewObserver observer(*this);

    auto it = std::find(m_contents.begin(), m_contents.end(), view);
    if (*it == m_contents.back() || it == m_contents.end())
        return;

    std::move(it, it + 1, m_contents.end());
}

ViewContext* ContentView::foregroundView()
{
    if (m_contents.empty())
        return nullptr;

    return m_contents[0].get();
}

void ContentView::setRenderClient(RenderClient* client)
{
    if (client == m_renderClient)
        return;

    m_renderClient = client;
}

void ContentView::motionSettingsChanged()
{
    m_motionDetector.reset();
}

void ContentView::contentInvalidated()
{
    if (!m_renderClient)
        return;

    m_renderClient->contentInvalidated();
}

float ContentView::deviceScaleFactor()
{
    return getContext().getResources().getDisplayMetrics().density;
}

void ContentView::onPause()
{
    propagate(m_contents, &ViewContext::pause);
    updateContentsRenderState();
}

void ContentView::onResume()
{
    propagate(m_contents, &ViewContext::resume);
    updateContentsRenderState();
}

bool ContentView::sendGenericEvent(UIEvent& event)
{
    if (!m_motionDetector.sendGenericEvent(event)) {
        if (hasOnClickListeners() && appkit::isClickEvent(event)) {
            callOnClick();
            return true;
        }

        return false;
    }

    return true;
}

bool ContentView::startInputMethod()
{
    std::shared_ptr<InputMethodManager> imm = std::static_pointer_cast<InputMethodManager>(
        getContext().getSystemService(Context::INPUT_METHOD_SERVICE));
    imm->restartInput(*this);
    return imm->showSoftInput(*this, 0);
}

bool ContentView::endInputMethod()
{
    std::shared_ptr<InputMethodManager> imm = std::static_pointer_cast<InputMethodManager>(
        getContext().getSystemService(Context::INPUT_METHOD_SERVICE));
    return imm->hideSoftInputFromWindow(getWindowToken(), 0);
}

void ContentView::detachViewFromRenderClient(std::shared_ptr<ViewContext> view)
{
    std::shared_ptr<RenderContext> renderer = view->renderer();
    m_renderClient->renderEventPosted([=] {
        renderer->teardown();
        m_handler->post([view] { });
    });
}

void ContentView::updateContentsRenderState()
{
    if (!m_renderClient)
        return;

    if (getVisibility() != View::VISIBLE)
        return;

    if (m_contents.size() == 0)
        return;

    std::vector<std::shared_ptr<RenderContext>> renderers;
    renderers.push_back(m_contents[0]->renderer());

    m_renderClient->renderListChanged(RenderContextList::create(std::move(renderers)));
}

void ContentView::setVisibility(int32_t visibility)
{
    View::setVisibility(visibility);
    if (m_renderClient)
        m_renderClient->contentVisibilityChanged(visibility);
    propagate(m_contents, &ViewContext::setVisibility, visibility == View::VISIBLE);
    updateContentsRenderState();
}

void ContentView::onAttachedToWindow()
{
    View::onAttachedToWindow();
    propagate(m_contents, &ViewContext::attached, *this);
    propagate(m_contents, &ViewContext::onLayout, getWidth(), getHeight());
    updateContentsRenderState();
}

void ContentView::onDetachedFromWindow()
{
    View::onDetachedFromWindow();
    propagate(m_contents, &ViewContext::onDetachedFromWindow);
    for (auto& content : m_contents)
        detachViewFromRenderClient(content);
    updateContentsRenderState();
}

void ContentView::onFocusChanged(bool gainFocus, int32_t direction, Rect& previouslyFocusedRect)
{
    View::onFocusChanged(gainFocus, direction, previouslyFocusedRect);

    if (!foregroundView())
        return;

    foregroundView()->setFocus(gainFocus);
    updateContentsRenderState();
}

void ContentView::onMeasure(int32_t widthMeasureSpec, int32_t heightMeasureSpec)
{
    View::onMeasure(widthMeasureSpec, heightMeasureSpec);
}

void ContentView::onLayout(Rect& rc)
{
    View::onLayout(rc);
    propagate(m_contents, &ViewContext::onLayout, getWidth(), getHeight());
    updateContentsRenderState();
}

void ContentView::onConfigurationChanged(Configuration& config)
{
    propagate(m_contents, &ViewContext::onConfigurationChanged, config);
}

std::shared_ptr<EditableContext> ContentView::editableContext()
{
    if (!foregroundView())
        return nullptr;

    return foregroundView()->editable();
}

MotionDetector::Settings ContentView::clientSettings()
{
    if (!foregroundView())
        return MotionDetector::Settings();

    return foregroundView()->motionSettings();
}

int32_t ContentView::clientWidth()
{
    return getWidth();
}

int32_t ContentView::clientHeight()
{
    return getHeight();
}

float ContentView::clientScaleFactor()
{
    if (!foregroundView())
        return 1.0f;

    return foregroundView()->currentScale();
}

float ContentView::clientMinimumScaleFactor()
{
    if (!foregroundView())
        return 1.0f;

    return foregroundView()->minimumScale();
}

float ContentView::clientMaximumScaleFactor()
{
    if (!foregroundView())
        return 1.0f;

    return foregroundView()->maximumScale();
}

int32_t ContentView::contentWidth()
{
    if (!foregroundView())
        return 0;

    return foregroundView()->contentWidth();
}

int32_t ContentView::contentHeight()
{
    if (!foregroundView())
        return 0;

    return foregroundView()->contentHeight();
}

RectF ContentView::contentViewport()
{
    if (!foregroundView())
        return RectF();

    return foregroundView()->viewport();
}

void ContentView::handleSingleTap(float x, float y)
{
    if (!foregroundView())
        return;

    return foregroundView()->sendSingleTap(x, y);
}

void ContentView::handleSingleTapUnconfirmed(float x, float y)
{
}

void ContentView::handleDoubleTap(float x, float y)
{
    if (!foregroundView())
        return;

    return foregroundView()->sendDoubleTap(x, y);
}

void ContentView::handleLongTap(float x, float y)
{
}

void ContentView::handleShowPressState(float x, float y)
{
}

void ContentView::handleShowPressCancel(float x, float y)
{
}

void ContentView::handleLongPress(float x, float y)
{
}

void ContentView::handleScrollBegin(float x, float y)
{
}

void ContentView::handleScrollBy(float x, float y, float deltaX, float deltaY)
{
    if (!foregroundView())
        return;

    foregroundView()->scrollBy(deltaX, deltaY);

    contentInvalidated();
}

void ContentView::handleScrollEnd(float x, float y)
{
}

void ContentView::handleFlingStart(float x, float y, int32_t vx, int32_t vy)
{
}

void ContentView::handleFlingCancel()
{
}

void ContentView::handlePinchBegin(float x, float y)
{
    if (!foregroundView())
        return;

    foregroundView()->pinchBegin(x, y);
}

void ContentView::handlePinchTo(RectF& rect)
{
    if (!foregroundView())
        return;

    foregroundView()->pinchTo(rect);

    contentInvalidated();
}

void ContentView::handlePinchBy(float x, float y, float deltaScale)
{
    if (!foregroundView())
        return;

    foregroundView()->pinchBy(x, y, deltaScale);

    contentInvalidated();
}

void ContentView::handlePinchEnd()
{
    if (!foregroundView())
        return;

    foregroundView()->pinchEnd();
}

bool ContentView::handleUnconfirmed(UIEvent& event)
{
    if (!foregroundView())
        return false;

    return foregroundView()->sendGenericEvent(event);
}

} // namespace appkit
} // namespace view
} // namespace android
