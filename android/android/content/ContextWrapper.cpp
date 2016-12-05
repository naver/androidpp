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

#include "ContextWrapper.h"

#include <android/content/ContextPrivate.h>
#include <android/view/inputmethod/InputMethodManager.h>

namespace android {
namespace content {

ContextWrapper::ContextWrapper(Context* base)
    : m_base(base)
{
}

ContextWrapper::~ContextWrapper()
{
}

Context& ContextWrapper::getApplicationContext()
{
    if (m_base)
        return m_base->getApplicationContext();

    return *this;
}

std::shared_ptr<Object> ContextWrapper::getSystemService(const String& name)
{
    if (m_base)
        return m_base->getSystemService(name);

    if (name.compare(Context::INPUT_METHOD_SERVICE) == 0) {
        static std::shared_ptr<InputMethodManager> imm = std::make_shared<InputMethodManager>();
        return imm;
    }

    return nullptr;
}

} // namespace content
} // namespace android
