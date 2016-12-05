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

#include "Bitmap.h"

#include <platforms/Functional.h>

#include <cairo/cairo.h>

namespace android {
namespace graphics {

class BitmapPrivate {
public:
    BitmapPrivate(const std::vector<int32_t>& colors, int32_t width, int32_t height, Bitmap::Config config);
    void setPixels(const std::vector<int32_t>& pixels, int32_t offset, int32_t stride, int32_t x, int32_t y, int32_t width, int32_t height);
    void getPixels(std::vector<int32_t>& pixels, int32_t offset, int32_t stride, int32_t x, int32_t y, int32_t width, int32_t height);

    int32_t getWidth();
    int32_t getHeight();
    int32_t getRowBytes();
    int32_t getByteCount() { return getRowBytes() * getHeight(); }

    std::unique_ptr<cairo_surface_t, std::function<void(cairo_surface_t*)>> m_image;
};

static cairo_format_t configToCairoFormat(Bitmap::Config config)
{
    switch (config) {
    case Bitmap::Config::ALPHA_8:
        return CAIRO_FORMAT_A8;
    case Bitmap::Config::RGB_565:
        return CAIRO_FORMAT_RGB16_565;
    case Bitmap::Config::ARGB_8888:
        return CAIRO_FORMAT_ARGB32;
    default:
        return CAIRO_FORMAT_INVALID;
    }
}

BitmapPrivate::BitmapPrivate(const std::vector<int32_t>& colors, int32_t width, int32_t height, Bitmap::Config config)
{
    const cairo_format_t cairoFormat = configToCairoFormat(config);
    if (cairoFormat == CAIRO_FORMAT_INVALID)
        return;

    int32_t stride = cairo_format_stride_for_width(cairoFormat, width);
    int32_t byteCount = stride * height;
    unsigned char* data = new unsigned char[byteCount];
    memcpy(data, colors.data(), byteCount);

    m_image = std::unique_ptr<cairo_surface_t, std::function<void(cairo_surface_t*)>>(
        cairo_image_surface_create_for_data(data, cairoFormat, width, height, stride),
        [data](cairo_surface_t* surface) { cairo_surface_destroy(surface); delete data; });

    cairo_status_t status = cairo_surface_status(m_image.get());
    if (status != CAIRO_STATUS_SUCCESS)
        m_image = nullptr;
}

void BitmapPrivate::setPixels(const std::vector<int32_t>& pixels, int32_t offset, int32_t stride, int32_t x, int32_t y, int32_t width, int32_t height)
{
    if (!m_image)
        return;

    if (getByteCount() < pixels.size() * 4)
        return;

    cairo_surface_flush(m_image.get());

    unsigned char* data = cairo_image_surface_get_data(m_image.get());
    // FIXME: Have to consider parameters instead of copying all pixels.
    memcpy(data, pixels.data(), pixels.size() * 4);

    cairo_surface_mark_dirty(m_image.get());
}

void BitmapPrivate::getPixels(std::vector<int32_t>& pixels, int32_t offset, int32_t stride, int32_t x, int32_t y, int32_t width, int32_t height)
{
    if (!m_image)
        return;

    if (pixels.size() * 4 < getByteCount())
        return;

    cairo_surface_flush(m_image.get());

    unsigned char* data = cairo_image_surface_get_data(m_image.get());
    // FIXME: Have to consider parameters instead of copying all pixels.
    memcpy(pixels.data(), data, getByteCount());
}

int32_t BitmapPrivate::getWidth()
{
    if (!m_image)
        return 0;

    return cairo_image_surface_get_width(m_image.get());
}

int32_t BitmapPrivate::getHeight()
{
    if (!m_image)
        return 0;

    return cairo_image_surface_get_height(m_image.get());
}

int32_t BitmapPrivate::getRowBytes()
{
    if (!m_image)
        return 0;

    return cairo_image_surface_get_stride(m_image.get());
}

std::shared_ptr<Bitmap> Bitmap::createBitmap(const std::vector<int32_t>& colors, int32_t width, int32_t height, Bitmap::Config config)
{
    return std::make_shared<Bitmap>(colors, width, height, config);
}

Bitmap::Bitmap(const std::vector<int32_t>& colors, int32_t width, int32_t height, Bitmap::Config config)
    : m_private(std::make_unique<BitmapPrivate>(colors, width, height, config))
{
}

Bitmap::~Bitmap()
{
}

void Bitmap::setPixels(const std::vector<int32_t>& pixels, int32_t offset, int32_t stride, int32_t x, int32_t y, int32_t width, int32_t height)
{
    m_private->setPixels(pixels, offset, stride, x, y, width, height);
}

void Bitmap::getPixels(std::vector<int32_t>& pixels, int32_t offset, int32_t stride, int32_t x, int32_t y, int32_t width, int32_t height)
{
    m_private->getPixels(pixels, offset, stride, x, y, width, height);
}

int32_t Bitmap::getWidth()
{
    return m_private->getWidth();
}

int32_t Bitmap::getHeight()
{
    return m_private->getHeight();
}

int32_t Bitmap::getRowBytes()
{
    return m_private->getRowBytes();
}

int32_t Bitmap::getByteCount()
{
    return getRowBytes() * getHeight();
}

} // namespace graphics
} // namespace android
