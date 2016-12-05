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

#include "MediaPlayer.h"

#include <android/media/MediaPlayerPrivate.h>

#include <algorithm>

#include <assert>

namespace android {
namespace media {

std::shared_ptr<MediaPlayer> MediaPlayer::create()
{
    return std::shared_ptr<MediaPlayer>(new MediaPlayer);
}

std::shared_ptr<MediaPlayer> MediaPlayer::create(String& uri)
{
    return std::shared_ptr<MediaPlayer>(new MediaPlayer);
}

MediaPlayer::MediaPlayer()
    : m_private(MediaPlayerPrivate::create(*this))
    , m_state(MediaPlayerPrivate::Idle)
{
}

MediaPlayer::~MediaPlayer()
{
}

void MediaPlayer::addTimedTextSource(String& path, String& mimeType)
{
}

void MediaPlayer::deselectTrack(int32_t index)
{
}

int32_t MediaPlayer::getCurrentPosition()
{
    return m_private->getCurrentPosition();
}

int32_t MediaPlayer::getDuration()
{
    return m_private->getDuration();
}

int32_t MediaPlayer::getSelectedTrack(int32_t trackType)
{
    return 0;
}

std::vector<MediaPlayer::TrackInfo> MediaPlayer::getTrackInfo()
{
    return std::vector<MediaPlayer::TrackInfo>();
}

int32_t MediaPlayer::getVideoHeight()
{
    return m_private->getVideoHeight();
}

int32_t MediaPlayer::getVideoWidth()
{
    return m_private->getVideoWidth();
}

bool MediaPlayer::isLooping()
{
    return false;
}

bool MediaPlayer::isPlaying()
{
    return m_state == MediaPlayerPrivate::Started;
}

void MediaPlayer::pause()
{
    assert(m_state == MediaPlayerPrivate::Started || m_state == MediaPlayerPrivate::Paused || m_state == MediaPlayerPrivate::PlaybackCompleted);

    stateChanged(MediaPlayerPrivate::Paused);
}

void MediaPlayer::prepare()
{
    // Do NOT use prepare(), since it works synchronously.
    assert(0);
}

void MediaPlayer::prepareAsync()
{
    assert(m_state == MediaPlayerPrivate::Initialized || m_state == MediaPlayerPrivate::Stopped);

    stateChanged(MediaPlayerPrivate::Preparing);
}

void MediaPlayer::release()
{
}

void MediaPlayer::reset()
{
    stateChanged(MediaPlayerPrivate::Idle);
}

void MediaPlayer::seekTo(int32_t msec)
{
    assert(m_state == MediaPlayerPrivate::Prepared || m_state == MediaPlayerPrivate::Started || m_state == MediaPlayerPrivate::Paused || m_state == MediaPlayerPrivate::PlaybackCompleted);

    msec = std::max(0, std::min(msec, m_private->getDuration()));
    m_private->seekTo(msec);
}

void MediaPlayer::selectTrack(int32_t index)
{
}

void MediaPlayer::setDataSource(String& path)
{
    assert(m_state == MediaPlayerPrivate::Idle);

    m_private->setDataSource(path);
    stateChanged(MediaPlayerPrivate::Initialized);
}

void MediaPlayer::setDataSource(Context& context, String& uri, const std::map<String, String>& headers)
{
}

void MediaPlayer::setLooping(bool looping)
{
}

void MediaPlayer::setOnBufferingUpdateListener(OnBufferingUpdateListener listener)
{
    m_bufferingUpdateListener = std::move(listener);
}

void MediaPlayer::setOnCompletionListener(OnCompletionListener listener)
{
    m_completionListener = std::move(listener);
}

void MediaPlayer::setOnErrorListener(OnErrorListener listener)
{
    m_errorListener = std::move(listener);
}
    
void MediaPlayer::setOnInfoListener(OnInfoListener listener)
{
    m_infoListener = std::move(listener);
}

void MediaPlayer::setOnPreparedListener(OnPreparedListener listener)
{
    m_preparedListener = std::move(listener);
}

void MediaPlayer::setOnSeekCompleteListener(OnSeekCompleteListener listener)
{
    m_seekCompleteListener = std::move(listener);
}

void MediaPlayer::setOnTimedTextListener(OnTimedTextListener listener)
{
    m_timedTextListener = std::move(listener);
}

void MediaPlayer::setOnVideoSizeChangedListener(OnVideoSizeChangedListener listener)
{
    m_videoSizeChangedListener = std::move(listener);
}

void MediaPlayer::setScreenOnWhilePlaying(bool screenOn)
{
}

void MediaPlayer::setVideoScalingMode(int32_t mode)
{
}

void MediaPlayer::setVolume(float leftVolume, float rightVolume)
{
    m_private->setVolume(leftVolume);
}

void MediaPlayer::setWakeMode(int32_t mode)
{
}

void MediaPlayer::start()
{
    assert(m_state == MediaPlayerPrivate::Prepared || m_state == MediaPlayerPrivate::Started || m_state == MediaPlayerPrivate::Paused || m_state == MediaPlayerPrivate::PlaybackCompleted);

    stateChanged(MediaPlayerPrivate::Started);
}

void MediaPlayer::stop()
{
    assert(m_state == MediaPlayerPrivate::Prepared || m_state == MediaPlayerPrivate::Started || m_state == MediaPlayerPrivate::Stopped || m_state == MediaPlayerPrivate::Paused || m_state == MediaPlayerPrivate::PlaybackCompleted);

    stateChanged(MediaPlayerPrivate::Stopped);
}

static MediaPlayerPrivate::State toState(int32_t state)
{
    switch (state) {
    case MediaPlayerPrivate::Unknown:
    case MediaPlayerPrivate::Idle:
    case MediaPlayerPrivate::Initialized:
    case MediaPlayerPrivate::Preparing:
    case MediaPlayerPrivate::Prepared:
    case MediaPlayerPrivate::Started:
    case MediaPlayerPrivate::Paused:
    case MediaPlayerPrivate::Stopped:
    case MediaPlayerPrivate::PlaybackCompleted:
    case MediaPlayerPrivate::End:
    case MediaPlayerPrivate::Error:
        return static_cast<MediaPlayerPrivate::State>(state);
    default:
        assert(false);
        break;
    };
    return MediaPlayerPrivate::Error;
}

void MediaPlayer::stateChanged(int32_t newState)
{
    MediaPlayerPrivate::State oldState = toState(m_state);
    m_state = newState;

    m_private->stateChanged(oldState, toState(newState));
}

} // namespace media
} // namespace android
