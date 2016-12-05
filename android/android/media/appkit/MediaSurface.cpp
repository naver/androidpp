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

#include "MediaSurface.h"

#include <android/media/MediaPlayerPrivate.h>

namespace android {
namespace media {
namespace appkit {

std::shared_ptr<MediaSurface> MediaSurface::create()
{
    return std::shared_ptr<MediaSurface>(new MediaSurface);
}

MediaSurface::MediaSurface()
{
}

MediaSurface::~MediaSurface()
{
    reset();
}

void MediaSurface::setSourceMedia(std::shared_ptr<MediaPlayer>& mediaPlayer)
{
    if (mediaPlayer == m_mediaPlayer)
        return;

    detachCurrentSourceMedia();

    m_mediaPlayer = mediaPlayer;

    attachCurrentSourceMedia();
}

void MediaSurface::setOutputSurface(std::shared_ptr<GLTransportSurface> surface)
{
    if (surface == m_outputSurface)
        return;

    if (m_outputSurface)
        m_outputSurface->setBackbuffer(nullptr);

    m_outputSurface = surface;

    if (m_outputSurface)
        m_outputSurface->setBackbuffer(this);
}

void MediaSurface::setOnFrameAvailableListener(OnFrameAvailableListener listener)
{
    m_frameAvailableListener = std::move(listener);
}

void MediaSurface::reset()
{
    detachCurrentSourceMedia();
    if (m_outputSurface)
        m_outputSurface->setBackbuffer(nullptr);
    m_mediaPlayer.reset();
    m_outputSurface.reset();
    m_frameAvailableListener.reset();
}

void MediaSurface::swapBuffers(GLTransportSurface&)
{
    std::shared_ptr<GLTexture> frame = frameTexture();
    if (!frame)
        return;

    m_outputSurface->copyFromTexture(*frame, true);
}

void MediaSurface::frameAvailable()
{
    m_frameAvailableListener(*this);
}

void MediaSurface::attachCurrentSourceMedia()
{
    if (!m_mediaPlayer)
        return;

    MediaPlayerPrivate& mediaPlayer = MediaPlayerPrivate::getPrivate(*m_mediaPlayer);
    mediaPlayer.setOnFrameAvailableListener([=] { frameAvailable(); });
}

void MediaSurface::detachCurrentSourceMedia()
{
    if (!m_mediaPlayer)
        return;

    MediaPlayerPrivate& mediaPlayer = MediaPlayerPrivate::getPrivate(*m_mediaPlayer);
    mediaPlayer.setOnFrameAvailableListener(nullptr);
}

std::shared_ptr<GLTexture> MediaSurface::frameTexture()
{
    if (!m_mediaPlayer)
        return nullptr;

    MediaPlayerPrivate& mediaPlayer = MediaPlayerPrivate::getPrivate(*m_mediaPlayer);
    std::shared_ptr<GLTexture> texture = GLTexture::create();
    mediaPlayer.copyFrame(*texture);
    return std::move(texture);
}

} // namespace appkit
} // namespace media
} // namespace android
