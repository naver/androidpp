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

#include <java/lang/CharSequence.h>
#include <android/view/KeyEvent.h>

namespace android {
namespace view {
namespace inputmethod {

class InputConnectionPrivate;

class InputConnection {
public:
    // The editor is requested to call updateCursorAnchorInfo(android.view.View, CursorAnchorInfo) at once, as soon as possible, regardless of cursor/anchor position changes.
    static const int32_t CURSOR_UPDATE_IMMEDIATE = 1;
    // The editor is requested to call updateCursorAnchorInfo(android.view.View, CursorAnchorInfo) whenever cursor/anchor position is changed.
    static const int32_t CURSOR_UPDATE_MONITOR = 2;

    // Commit text to the text box and set the new cursor position. 
    ANDROID_EXPORT virtual bool commitText(CharSequence& text, int32_t newCursorPosition) = 0;
    // Delete beforeLength characters of text before the current cursor position, and delete afterLength characters of text after the current cursor position, excluding the selection. 
    ANDROID_EXPORT virtual bool deleteSurroundingText(int32_t beforeLength, int32_t afterLength) = 0;
    // Gets the selected text, if any. 
    ANDROID_EXPORT virtual CharSequence getSelectedText(int32_t flags) = 0;
    // Have the text editor finish whatever composing text is currently active. 
    ANDROID_EXPORT virtual bool finishComposingText() = 0;
    // Called by the input method to ask the editor for calling back updateCursorAnchorInfo(android.view.View, CursorAnchorInfo) to notify cursor/anchor locations.
    ANDROID_EXPORT virtual bool requestCursorUpdates(int32_t cursorUpdateMode) = 0;
    // Replace the currently composing text with the given text, and set the new cursor position.
    ANDROID_EXPORT virtual bool setComposingText(CharSequence& text, int32_t newCursorPosition) = 0;
    // Send a key event to the process that is currently attached through this input connection.
    ANDROID_EXPORT virtual bool sendKeyEvent(KeyEvent& event) = 0;

    ANDROID_EXPORT virtual ~InputConnection();

private:
    std::unique_ptr<InputConnectionPrivate> m_private;
};

} // namespace inputmethod
} // namespace view
} // namespace android

using InputConnection = android::view::inputmethod::InputConnection;
