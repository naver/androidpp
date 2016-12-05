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

#include "GLTransportSurface.h"

#include "GLTextureStreamChannel.h"
#include "GLTextureStreamHost.h"

#include <assert>

namespace android {
namespace opengl {
namespace appkit {

std::shared_ptr<GLTransportSurface> GLTransportSurface::import(EGLConfig config, int32_t shareIdentifier)
{
    std::shared_ptr<MessageHost> host = MessageHost::get(shareIdentifier);
    if (!host)
        return nullptr;

    GLTextureStreamHost* textureHost = static_cast<GLTextureStreamHost*>(host.get());
    return std::move(textureHost->import(config));
}

GLTransportSurface::GLTransportSurface()
    : m_surfaceType(Unknown)
    , m_backbuffer(nullptr)
{
}

GLTransportSurface::~GLTransportSurface()
{
    m_channel.reset();
}

void GLTransportSurface::setBackbuffer(Backbuffer* backbuffer)
{
    if (backbuffer == m_backbuffer)
        return;

    m_backbuffer = backbuffer;
}

void GLTransportSurface::swapBuffers()
{
    if (!m_backbuffer)
        return;

    m_backbuffer->swapBuffers(*this);
}

void GLTransportSurface::connect()
{
    if (m_channel)
        return;

    m_channel = std::move(GLTextureStreamChannel::create(*this));
    m_channel->connect();
}

void GLTransportSurface::reshape(EGLint width, EGLint height)
{
    assert(m_channel);

    m_channel->reshape(width, height);
}

void GLTransportSurface::publish(Type surfaceType, intptr_t shareHandle)
{
    assert(m_channel);

    m_surfaceType = surfaceType;

    m_channel->publish(surfaceType, shareHandle);
}

} // namespace appkit
} // namespace opengl
} // namespace android
