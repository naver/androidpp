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

#undef DEBUG
#undef ERROR
#undef ASSERT

namespace android {
namespace util {

class Log {
public:
    static const int32_t VERBOSE    = 2;
    static const int32_t DEBUG      = 3;
    static const int32_t INFO       = 4;
    static const int32_t WARN       = 5;
    static const int32_t ERROR      = 6;
    static const int32_t ASSERT     = 7;

    // Checks to see whether or not a log for the specified tag is loggable at the specified level.
    ANDROID_EXPORT static bool isLoggable(const char* tag, int32_t level);

    // Send a VERBOSE log message.
    ANDROID_EXPORT static int32_t v(const char* tag, const char* msg);
    // Send a DEBUG log message.
    ANDROID_EXPORT static int32_t d(const char* tag, const char* msg);
    // Send an INFO log message.
    ANDROID_EXPORT static int32_t i(const char* tag, const char* msg);
    // Send a WARN log message.
    ANDROID_EXPORT static int32_t w(const char* tag, const char* msg);
    // Send an ERROR log message.
    ANDROID_EXPORT static int32_t e(const char* tag, const char* msg);
    // What a Terrible Failure: Report a condition that should never happen. 
    ANDROID_EXPORT static int32_t wtf(const char* tag, const char* msg);

    // Low-level logging call.
    ANDROID_EXPORT static int32_t println(int32_t priority, const char* tag, const char* msg);

private:
    static int32_t println(const char* tag, const char* msg);
};

} // namespace util
} // namespace android

using Log = android::util::Log;
