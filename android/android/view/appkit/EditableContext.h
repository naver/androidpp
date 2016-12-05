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

#include <android/graphics/Rect.h>
#include <android/view/appkit/CompositionClause.h>
#include <android/view/appkit/UIEvent.h>

#include <vector>

namespace android {
namespace view {
namespace appkit {

class EditableContext {
public:
    enum class EditorAction {
        Unspecified,
        None,
        Go,
        Search,
        Send,
        Next,
        Done,
        Previous
    };

    virtual ~EditableContext() = default;

    virtual Rect editableCaretRectAtStart() = 0;
    virtual Rect editableCaretRectAtEnd() = 0;
    virtual std::wstring editableSelectionText() = 0;
    virtual std::wstring editableTextBeforeCaret(int32_t) = 0;
    virtual std::wstring editableTextAfterCaret(int32_t) = 0;

    virtual bool setComposition(const std::wstring&, const std::vector<CompositionClause>&, int32_t, int32_t, int32_t, int32_t) = 0;
    virtual bool confirmComposition(const std::wstring&) = 0;
    virtual bool finishComposition() = 0;
    virtual void cancelComposition() = 0;

    virtual bool performEditorAction(EditorAction) = 0;
    virtual bool sendKeyEvent(UIEvent&) = 0;

protected:
    EditableContext() = default;
};

} // namespace appkit
} // namespace view
} // namespace android

using EditableContext = android::view::appkit::EditableContext;
