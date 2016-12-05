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

#include <vector>

namespace android {
namespace view {

class ViewGroup : public View {
public:
    ANDROID_EXPORT ViewGroup();
    ANDROID_EXPORT virtual ~ViewGroup();

    // Adds a child view.
    ANDROID_EXPORT virtual void addView(std::shared_ptr<View>);
    // Change the z order of the child so it's on top of all other children.
    ANDROID_EXPORT virtual void bringChildToFront(std::shared_ptr<View>);

    ANDROID_EXPORT virtual View* findFocus() override;
    // Returns the view at the specified position in the group. 
    ANDROID_EXPORT virtual View* getChildAt(int index);

    ANDROID_EXPORT virtual void setVisibility(int32_t) override;

    ANDROID_EXPORT virtual void onWindowFocusChanged(bool hasWindowFocus) override;

protected:
    ANDROID_EXPORT virtual void onAttachedToWindow() override;
    ANDROID_EXPORT virtual void onDetachedFromWindow() override;
    ANDROID_EXPORT virtual void onWindowVisibilityChanged(int visibility) override;
    ANDROID_EXPORT virtual void onFocusChanged(bool gainFocus, int32_t direction, Rect& previouslyFocusedRect) override;
    ANDROID_EXPORT virtual void onMeasure(int32_t, int32_t) override;
    ANDROID_EXPORT virtual void onLayout(Rect&) override;
    ANDROID_EXPORT virtual void onConfigurationChanged(Configuration&) override;

private:
    std::vector<std::shared_ptr<View>> m_children;
};

} // namespace view
} // namespace android

using ViewGroup = android::view::ViewGroup;
