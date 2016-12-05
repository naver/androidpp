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

#include "GLTextureStreamChannel.h"

#include "GLTextureStreamChannelMessages.h"
#include "GLTextureStreamHostMessages.h"
#include "GLTransportSurface.h"

#include <android/os/appkit/MessageChannelMessages.h>
#include <android/os/appkit/Process.h>

#include <assert>

namespace android {
namespace opengl {
namespace appkit {

std::unique_ptr<GLTextureStreamChannel> GLTextureStreamChannel::create(GLTransportSurface& surface)
{
    return std::unique_ptr<GLTextureStreamChannel>(new GLTextureStreamChannel(surface));
}

GLTextureStreamChannel::GLTextureStreamChannel(GLTransportSurface& surface)
    : m_surface(surface)
{
}

GLTextureStreamChannel::~GLTextureStreamChannel()
{
}

void GLTextureStreamChannel::connect()
{
    MessageChannel::connect(GLTextureStreamHostMessages::get().CONNECT);
}

void GLTextureStreamChannel::reshape(EGLint width, EGLint height)
{
    send(GLTextureStreamHostMessages::get().Reshape(width, height), GLTextureStreamChannelMessages::get().RESHAPED);
}

void GLTextureStreamChannel::publish(GLTransportSurface::Type surfaceType, intptr_t shareHandle)
{
    send(GLTextureStreamHostMessages::get().Publish(surfaceType, shareHandle), GLTextureStreamChannelMessages::get().PUBLISHED);
}

void GLTextureStreamChannel::receive(const Message& message)
{
    if (message.what == GLTextureStreamChannelMessages::get().RESHAPED) {
        return;
    } else if (message.what == GLTextureStreamChannelMessages::get().PUBLISHED) {
        return;
    } else {
        MessageChannel::receive(message);
    }
}

} // namespace appkit
} // namespace opengl
} // namespace android
