/*
 Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies)
 Copyright (C) 2012 Igalia S.L.
 Copyright (C) 2012 Adobe Systems Incorporated

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Library General Public
 License as published by the Free Software Foundation; either
 version 2 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Library General Public License for more details.

 You should have received a copy of the GNU Library General Public License
 along with this library; see the file COPYING.LIB.  If not, write to
 the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 Boston, MA 02110-1301, USA.
 */

#include "GLTexture.h"

#include "GLExtensions.h"

#include <vector>

#include <assert>

#define GL_UNPACK_ROW_LENGTH 0x0CF2
#define GL_UNPACK_SKIP_PIXELS 0x0CF4
#define GL_UNPACK_SKIP_ROWS 0x0CF3

#if defined(__APPLE__)
#define DEFAULT_TEXTURE_PIXEL_TRANSFER_TYPE GL_UNSIGNED_INT_8_8_8_8_REV
#else
#define DEFAULT_TEXTURE_PIXEL_TRANSFER_TYPE GL_UNSIGNED_BYTE
#endif

namespace android {
namespace opengl {
namespace appkit {

std::shared_ptr<GLTexture> GLTexture::create()
{
    return std::shared_ptr<GLTexture>(new GLTexture);
}

GLTexture::GLTexture()
    : GLTextureHandle(0)
    , m_width(0)
    , m_height(0)
{
    ::glGenTextures(1, &m_id);
}

GLTexture::~GLTexture()
{
    ::glDeleteTextures(1, &m_id);
}

// If GL_EXT_texture_format_BGRA8888 is supported in the OpenGLES
// internal and external formats need to be BGRA
static bool driverSupportsExternalTextureBGRA()
{
    static bool supportsExternalTextureBGRA = GLExtensions::supportsExtension(L"GL_EXT_texture_format_BGRA8888");
    return supportsExternalTextureBGRA;
}

static bool driverSupportsSubImage()
{
    static bool supportsSubImage = GLExtensions::supportsExtension(L"GL_EXT_unpack_subimage");
    return supportsSubImage;
}

static void swizzleBGRAToRGBA(uint32_t* data, Rect& rect, int32_t stride = 0)
{
    stride = stride ? stride : rect.width();
    for (int y = rect.top; y < rect.bottom; ++y) {
        uint32_t* p = data + y * stride;
        for (int x = rect.left; x < rect.right; ++x)
            p[x] = ((p[x] << 16) & 0xff0000) | ((p[x] >> 16) & 0xff) | (p[x] & 0xff00ff00);
    }
}

static const unsigned bytesPerPixel = sizeof(int32_t);

void GLTexture::image2D(int32_t width, int32_t height, Format format, const int32_t* sourceData)
{
    ::glBindTexture(GL_TEXTURE_2D, m_id);
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    bool requireImageBuffer = false;
    if (!driverSupportsExternalTextureBGRA() && format == Format::BGRA) {
        requireImageBuffer = true;
        format = Format::RGBA;
    }

    if (!sourceData) {
        ::glTexImage2D(GL_TEXTURE_2D, 0, (GLint)format, width, height, 0, (GLenum)format, DEFAULT_TEXTURE_PIXEL_TRANSFER_TYPE, 0);
        return;
    }

    char* data = reinterpret_cast<char*>(const_cast<int32_t*>(sourceData));
    std::vector<char> temporaryData;
    const unsigned bytesPerLine = width * sizeof(int32_t);

    if (requireImageBuffer) {
        temporaryData.resize(width * height * bytesPerPixel);
        data = temporaryData.data();
        swizzleBGRAToRGBA(reinterpret_cast<uint32_t*>(data), Rect(width, height), bytesPerLine / bytesPerPixel);
    }

    ::glTexImage2D(GL_TEXTURE_2D, 0, (GLint)format, width, height, 0, (GLenum)format, DEFAULT_TEXTURE_PIXEL_TRANSFER_TYPE, data);
}

void GLTexture::subImage2D(int32_t x, int32_t y, int32_t width, int32_t height, Format format, const int32_t* sourceData)
{
    upload(sourceData, Rect(Point(x, y), width, height), Point(0, 0), width * sizeof(int32_t), format, false);
}

void GLTexture::upload(const int32_t* sourceData, Rect& targetRect, Point& sourceOffset, int32_t bytesPerLine, Format format)
{
    ::glBindTexture(GL_TEXTURE_2D, m_id);
    if (driverSupportsSubImage()) { // For ES drivers that don't support sub-images.
        // Use the OpenGL sub-image extension, now that we know it's available.
        ::glPixelStorei(GL_UNPACK_ROW_LENGTH, bytesPerLine / bytesPerPixel);
        ::glPixelStorei(GL_UNPACK_SKIP_ROWS, sourceOffset.y);
        ::glPixelStorei(GL_UNPACK_SKIP_PIXELS, sourceOffset.x);
    }

    ::glTexSubImage2D(GL_TEXTURE_2D, 0, targetRect.left, targetRect.top, targetRect.width(), targetRect.height(), (GLenum)format, DEFAULT_TEXTURE_PIXEL_TRANSFER_TYPE, sourceData);

    if (driverSupportsSubImage()) { // For ES drivers that don't support sub-images.
        ::glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        ::glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
        ::glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    }
}

void GLTexture::upload(const int32_t* sourceData, Rect& targetRect, Point& sourceOffset, int32_t bytesPerLine, Format format, bool readOnly)
{
    ::glBindTexture(GL_TEXTURE_2D, m_id);

    char* data = reinterpret_cast<char*>(const_cast<int32_t*>(sourceData));
    std::vector<char> temporaryData;
    Point adjustedSourceOffset = sourceOffset;

    // Texture upload requires subimage buffer if driver doesn't support subimage and we don't have full image upload.
    bool requireSubImageBuffer = !driverSupportsSubImage()
        && !(bytesPerLine == static_cast<int32_t>(targetRect.width() * bytesPerPixel) && adjustedSourceOffset == Point());

    // prepare temporaryData if necessary
    if ((!driverSupportsExternalTextureBGRA() && readOnly) || requireSubImageBuffer) {
        temporaryData.resize(targetRect.width() * targetRect.height() * bytesPerPixel);
        data = temporaryData.data();
        const char* bits = reinterpret_cast<const char*>(sourceData);
        const char* src = bits + sourceOffset.y * bytesPerLine + sourceOffset.x * bytesPerPixel;
        char* dst = data;
        const int targetBytesPerLine = targetRect.width() * bytesPerPixel;
        for (int y = 0; y < targetRect.height(); ++y) {
            memcpy(dst, src, targetBytesPerLine);
            src += bytesPerLine;
            dst += targetBytesPerLine;
        }

        bytesPerLine = targetBytesPerLine;
        adjustedSourceOffset = Point();
    }

    if (!driverSupportsExternalTextureBGRA() && format == Format::BGRA) {
        swizzleBGRAToRGBA(reinterpret_cast<uint32_t*>(data), Rect(adjustedSourceOffset, targetRect.width(), targetRect.height()), bytesPerLine / bytesPerPixel);
        format = Format::RGBA;
    }

    upload(reinterpret_cast<int32_t*>(data), targetRect, adjustedSourceOffset, bytesPerLine, format);
}

} // namespace appkit
} // namespace opengl
} // namespace android
