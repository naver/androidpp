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

#if defined(WIN32)
#include <windows.h>
#endif

namespace android {
namespace graphics {

template<typename T>
class PointT {
public:
    PointT()
        : x(0), y(0)
    {
    }
    PointT(T _x, T _y)
        : x(_x), y(_y)
    {
    }
    PointT(const PointT& other)
        : x(other.x)
        , y(other.y)
    {
    }
    template <typename U> PointT(const PointT<U>& other)
        : x(static_cast<T>(other.x))
        , y(static_cast<T>(other.y))
    {
    }
#if defined(WIN32)
    PointT(const POINT& pt)
        : x(pt.x), y(pt.y)
    {
    }

    operator POINT() const
    {
        return { x, y };
    }
#endif

    void set(T _x, T _y)
    {
        x = _x;
        y = _y;
    }
    void offset(T dx, T dy)
    {
        x += dx;
        y += dy;
    }

    bool isEqual(const PointT& rhs) const
    {
        return x == rhs.x && y == rhs.y;
    }

    static_assert(std::is_pod<T>::value, "Type T is not a POD type.");

    T x;
    T y;
};

template<typename T> static bool operator==(const PointT<T>& a, const PointT<T>& b)
{
    return a.isEqual(b);
}

template<typename T> static bool operator!=(const PointT<T>& a, const PointT<T>& b)
{
    return !a.isEqual(b);
}

using Point = PointT<int32_t>;

} // namespace graphics
} // namespace android

using Point = android::graphics::Point;
