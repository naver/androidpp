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

#include <android/os/IBinder.h>
#include <android/view/inputmethod/CursorAnchorInfo.h>

namespace android {
namespace view {

class View;

namespace inputmethod {

class InputMethodManager final : public Object {
public:
    // Synonym for showSoftInput(View, int, ResultReceiver) without a result receiver: explicitly request that the current input method's soft input area be shown to the user, if needed.
    ANDROID_EXPORT bool showSoftInput(View& view, int32_t flags);
    // Synonym for hideSoftInputFromWindow(IBinder, int, ResultReceiver) without a result: request to hide the soft input window from the context of the window that is currently accepting input.
    ANDROID_EXPORT bool hideSoftInputFromWindow(IBinder windowToken, int32_t flags);
    // If the input method is currently connected to the given view, restart it with its new contents.
    ANDROID_EXPORT void restartInput(View& view);
    // Report positional change of the text insertion point and/or characters in the composition string.
    ANDROID_EXPORT void updateCursorAnchorInfo(View& view, CursorAnchorInfo& cursorAnchorInfo);
};

} // namespace inputmethod
} // namespace view
} // namespace android

using InputMethodManager = android::view::inputmethod::InputMethodManager;
