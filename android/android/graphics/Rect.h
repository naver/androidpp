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

#include <android/graphics/Point.h>

#if defined(WIN32)
#include <windows.h>
#endif

namespace android {
namespace graphics {

template<typename T>
class RectT {
public:
    RectT()
        : left(0), top(0), right(0), bottom(0)
    {
    }
    RectT(T width, T height)
        : left(0), top(0), right(width), bottom(height)
    {
    }
    RectT(T l, T t, T r, T b)
        : left(l), top(t), right(r), bottom(b)
    {
    }
    RectT(const RectT& other)
        : left(other.left)
        , top(other.top)
        , right(other.right)
        , bottom(other.bottom)
    {
    }
    template <typename U> RectT(const RectT<U>& other)
        : left(static_cast<T>(other.left))
        , top(static_cast<T>(other.top))
        , right(static_cast<T>(other.right))
        , bottom(static_cast<T>(other.bottom))
    {
    }
    template <typename U> RectT(const PointT<U>& leftTop, U width, U height)
        : left(static_cast<T>(leftTop.x))
        , top(static_cast<T>(leftTop.y))
        , right(left + static_cast<T>(width))
        , bottom(top + static_cast<T>(height))
    {
    }
    template <typename U> RectT(const PointT<U>& leftTop, const PointT<U>& rightBottom)
        : left(static_cast<T>(leftTop.x))
        , top(static_cast<T>(leftTop.y))
        , right(static_cast<T>(rightBottom.x))
        , bottom(static_cast<T>(rightBottom.y))
    {
    }
#if defined(WIN32)
    RectT(const RECT& rc)
        : left(rc.left), top(rc.top), right(rc.right), bottom(rc.bottom)
    {
    }

    operator RECT() const
    {
        return { left, top, right, bottom };
    }
#endif

    bool isEmpty() { return width() == 0 && height() == 0; }

    T width() { return right - left; }
    T height() { return bottom - top; }

    void setWidth(T w) { right = left + w; }
    void setHeight(T h) { bottom = top + h; }

    void set(T _left, T _top, T _right, T _bottom)
    {
        left = _left;
        top = _top;
        right = _right;
        bottom = _bottom;
    }
    void set(RectT& src)
    {
        left = src.left;
        top = src.top;
        right = src.right;
        bottom = src.bottom;
    }
    void offset(T dx, T dy)
    {
        left += dx;
        top += dy;
        right += dx;
        bottom += dy;
    }
    void offsetTo(T newLeft, T newTop)
    {
        right += newLeft - left;
        bottom += newTop - top;
        left = newLeft;
        top = newTop;
    }

    void unite(T l, T t, T r, T b)
    {
        left = (left < l) ? left : l;
        top = (top < t) ? top : t;
        right = (right > r) ? right : r;
        bottom = (bottom > b) ? bottom : b;
    }
    void unite(RectT& other)
    {
        unite(other.left, other.top, other.right, other.bottom);
    }

    void inset(T deltaX, T deltaY)
    {
        left += delatX;
        top += delatY;
        right -= delatX;
        bottom -= delatY;
    }

    T centerX()
    {
        return left + width() / 2;
    }
    T centerY()
    {
        return top + height() / 2;
    }

    bool contains(int px, int py)
    {
        return px >= left && px < right && py >= top && py < bottom;
    }
    template<typename U> bool contains(const PointT<U>& point)
    {
        return contains(point.x, point.y);
    }

    bool isEqual(const RectT& rhs) const
    {
        return left == rhs.left && top == rhs.top && right == rhs.right && bottom == rhs.bottom;
    }

    String description()
    {
        return String("lt(") + std::to_string(left) + "," + std::to_string(top) + ") wh(" + std::to_string(width()) + "," + std::to_string(height()) + ")";
    }

    static_assert(std::is_pod<T>::value, "Type T is not a POD type.");

    T left;
    T top;
    T right;
    T bottom;
};

template<typename T> static bool operator==(const RectT<T>& a, const RectT<T>& b)
{
    return a.isEqual(b);
}

template<typename T> static bool operator!=(const RectT<T>& a, const RectT<T>& b)
{
    return !a.isEqual(b);
}

using Rect = RectT<int32_t>;

} // namespace graphics
} // namespace android

using Rect = android::graphics::Rect;
