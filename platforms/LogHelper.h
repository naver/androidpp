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

#include <android/util/Log.h>

#define LOG_PRIORITY_VERBOSE    android::util::Log::VERBOSE
#define LOG_PRIORITY_DEBUG      android::util::Log::DEBUG
#define LOG_PRIORITY_INFO       android::util::Log::INFO
#define LOG_PRIORITY_WARN       android::util::Log::WARN
#define LOG_PRIORITY_ERROR      android::util::Log::ERROR
#define LOG_PRIORITY_ASSERT     android::util::Log::ASSERT

#define LOGV(format, ...)       LOG(LOG_PRIORITY_VERBOSE, 0, format, __VA_ARGS__)
#define LOGD(format, ...)       LOG(LOG_PRIORITY_DEBUG,   0, format, __VA_ARGS__)
#define LOGI(format, ...)       LOG(LOG_PRIORITY_INFO,    0, format, __VA_ARGS__)
#define LOGW(format, ...)       LOG(LOG_PRIORITY_WARN,    0, format, __VA_ARGS__)
#define LOGE(format, ...)       LOG(LOG_PRIORITY_ERROR,   0, format, __VA_ARGS__)
#define LOGA(format, ...)       LOG(LOG_PRIORITY_ASSERT,  0, format, __VA_ARGS__)

#define FMT_POINT               "(%d, %d)"
#define ARG_POINT(pt)           (pt).x, (pt).y

#define FMT_RECT                FMT_POINT"-"FMT_POINT
#define ARG_RECT(rc)            (rc).left, (rc).top, (rc).right, (rc).bottom

#undef LOG

ANDROID_EXPORT int32_t LOG(int32_t priority, const char* tag, const char* format, ...);
