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

#include "MediaPlayerPrivate.h"

#include <android/media/MediaPlayer.h>
#include <android/media/mock/MediaPlayerPrivateMock.h>

namespace android {
namespace media {

static MediaPlayerPrivate* (*s_factory)(MediaPlayer&) = 0;

std::unique_ptr<MediaPlayerPrivate> MediaPlayerPrivate::create(MediaPlayer& mp)
{
    if (s_factory)
        return std::unique_ptr<MediaPlayerPrivate>(s_factory(mp));

    return std::make_unique<MediaPlayerPrivateMock>(mp);
}

void MediaPlayerPrivate::setFactory(MediaPlayerPrivate* (*factory)(MediaPlayer&))
{
    s_factory = factory;
}

void MediaPlayerPrivate::callOnBufferingUpdateListener(int32_t percent)
{
    m_player.m_bufferingUpdateListener(percent);
}

void MediaPlayerPrivate::callOnCompletionListener()
{
    m_player.stateChanged(PlaybackCompleted);
    m_player.m_completionListener();
}

void MediaPlayerPrivate::callOnErrorListener(int32_t error)
{
    m_player.stateChanged(Error);
    m_player.m_errorListener(error);
}

void MediaPlayerPrivate::callOnInfoListener()
{
    m_player.m_infoListener();
}

void MediaPlayerPrivate::callOnPreparedListener()
{
    m_player.stateChanged(Prepared);
    m_player.m_preparedListener();
}

void MediaPlayerPrivate::callOnSeekCompleteListener()
{
    m_player.m_seekCompleteListener();
}

void MediaPlayerPrivate::callOnTimedTextListener()
{
    m_player.m_timedTextListener();
}

void MediaPlayerPrivate::callOnVideoSizeChangedListener(int32_t width, int32_t height)
{
    m_player.m_videoSizeChangedListener(width, height);
}

} // namespace media
} // namespace android
