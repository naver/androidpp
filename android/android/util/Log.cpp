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

#include "Log.h"

namespace android {
namespace util {

bool Log::isLoggable(const char* tag, int32_t level)
{
    return true;
}

int32_t Log::v(const char* tag, const char* msg)
{
    if (!isLoggable(tag, VERBOSE))
        return 0;

    return Log::println(tag, msg);
}

int32_t Log::d(const char* tag, const char* msg)
{
    if (!isLoggable(tag, DEBUG))
        return 0;

    return Log::println(tag, msg);
}

int32_t Log::i(const char* tag, const char* msg)
{
    if (!isLoggable(tag, INFO))
        return 0;

    return Log::println(tag, msg);
}

int32_t Log::w(const char* tag, const char* msg)
{
    if (!isLoggable(tag, WARN))
        return 0;

    return Log::println(tag, msg);
}

int32_t Log::e(const char* tag, const char* msg)
{
    if (!isLoggable(tag, ERROR))
        return 0;

    return Log::println(tag, msg);
}

void WTFCrash()
{
    *(int *)(uintptr_t)0xbbadbeef = 0;
    // More reliable, but doesn't say BBADBEEF.
#if COMPILER(CLANG) || COMPILER(GCC)
    __builtin_trap();
#else
    ((void(*)())0)();
#endif
}

int32_t Log::wtf(const char* tag, const char* msg)
{
    int32_t result = Log::println(tag, msg);
    WTFCrash();
    return result;
}

int32_t Log::println(int32_t priority, const char* tag, const char* msg)
{
    switch (priority) {
    case VERBOSE:
        return v(tag, msg);
    case DEBUG:
        return d(tag, msg);
    case INFO:
        return i(tag, msg);
    case WARN:
        return w(tag, msg);
    case ERROR:
        return e(tag, msg);
    case ASSERT:
        return wtf(tag, msg);
    default:
        return d(tag, msg);
    }
}

} // namespace util
} // namespace android
