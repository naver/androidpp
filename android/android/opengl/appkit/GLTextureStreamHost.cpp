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

#include "GLTextureStreamHost.h"

#include "EGLPbufferSurface.h"
#include "GLTextureStreamChannelMessages.h"
#include "GLTextureStreamHostMessages.h"
#include <android/os/appkit/Process.h>
#include <android/os/appkit/Thread.h>

#include <assert>

namespace android {
namespace opengl {
namespace appkit {

GLTextureStreamHost::GLTextureStreamHost(std::shared_ptr<Messenger>&& messageSender)
    : MessageHost(std::move(messageSender))
    , m_width(0)
    , m_height(0)
    , m_surfaceType(GLTransportSurface::Unknown)
    , m_shareHandle(0)
{
}

GLTextureStreamHost::~GLTextureStreamHost()
{
}

void GLTextureStreamHost::initializeOnce()
{
    static bool onceFlag = false;

    setReceiveMessages(onceFlag, GLTextureStreamHostMessages::get());
}

std::shared_ptr<GLTransportSurface> GLTextureStreamHost::import(EGLConfig config)
{
    std::shared_ptr<GLTransportSurface> surface;
    switch (m_surfaceType) {
    case GLTransportSurface::EGLPbuffer:
        surface = EGLPbufferSurface::import(config, m_shareHandle, m_width, m_height);
        break;
    default:
        assert(false);
        return nullptr;
    }

    m_surface = surface;
    return std::move(surface);
}

void GLTextureStreamHost::receive(Message& message)
{
    if (message.what == GLTextureStreamHostMessages::get().RESHAPE) {
        m_width = message.arg1;
        m_height = message.arg2;
        send(GLTextureStreamChannelMessages::get().Reshaped());
    } else if (message.what == GLTextureStreamHostMessages::get().PUBLISH) {
        m_surfaceType = static_cast<GLTransportSurface::Type>(message.arg1);
        m_shareHandle = message.obj;
        protect();
        if (!m_surface.expired())
            m_surface.lock()->reset(m_shareHandle, m_width, m_height);
        send(GLTextureStreamChannelMessages::get().Published());
    } else {
        MessageHost::receive(message);
    }
}

void GLTextureStreamHost::setReceiveMessages(bool& onceFlag, Messages& messages)
{
    if (onceFlag)
        return;

    onceFlag = true;

    Thread::runOnMainThread([&] {
        Process::current().setMessageReceiver(GLTextureStreamHost::receiveMessage, messages);
    });
}

bool GLTextureStreamHost::receiveMessage(Messenger& replySender, Message& message)
{
    if (message.what == GLTextureStreamHostMessages::get().CONNECT) {
        new GLTextureStreamHost(std::shared_ptr<Messenger>(message.replyTo));
        return true;
    }

    return false;
}

} // namespace appkit
} // namespace opengl
} // namespace android
