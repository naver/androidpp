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

#include <java/lang.h>

namespace android {
namespace view {

class ANDROID_EXPORT MenuItem {
public:
    MenuItem(String& title = String(), bool enabled = false, bool checked = false, int32_t tag = -1)
        : m_title(title)
        , m_enabled(enabled)
        , m_checked(checked)
        , m_tag(tag)
    {
    }

    void setTitle(String& title) { m_title = title; }
    String& title() { return m_title; }

    void setEnabled(bool enabled) { m_enabled = enabled; }
    bool enabled() { return m_enabled; }

    void setChecked(bool checked) { m_checked = checked; }
    bool checked() { return m_checked; }

    void setTag(int32_t tag) { m_tag = tag; }
    int32_t tag() { return m_tag; }

private:
    String m_title;
    bool m_enabled;
    bool m_checked;
    int32_t m_tag;
};

} // namespace view
} // namespace android

using MenuItem = android::view::MenuItem;
