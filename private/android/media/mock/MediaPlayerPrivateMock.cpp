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

#include "MediaPlayerPrivateMock.h"

#include <android/os/appkit/Process.h>

#include <algorithm>

#include <assert>

namespace android {
namespace media {

MediaPlayerPrivateMock::MediaPlayerPrivateMock(MediaPlayer& mp)
    : MediaPlayerPrivate(mp)
    , m_state(Idle)
    , m_pendingCallbacks(0)
    , m_suppressCallbacks(false)
    , m_duration(0)
    , m_buffering(0)
    , m_currentTime(0)
    , m_currentPosition(0)
{
}

MediaPlayerPrivateMock::~MediaPlayerPrivateMock()
{
    reset();
}

void MediaPlayerPrivateMock::setDataSource(const String& path)
{
    m_path = path;
}

void MediaPlayerPrivateMock::setVolume(float)
{
}

int32_t MediaPlayerPrivateMock::getDuration()
{
    return m_duration;
}

int32_t MediaPlayerPrivateMock::getCurrentPosition()
{
    return (m_currentPosition) ? m_currentPosition : m_currentTime;
}

void MediaPlayerPrivateMock::seekTo(int32_t msec)
{
    m_currentPosition = msec;
    post([=] { onSeekComplete(msec); }, mediaSeekingLatency);
}

int32_t MediaPlayerPrivateMock::getVideoHeight()
{
    return mediaVideoHeight;
}

int32_t MediaPlayerPrivateMock::getVideoWidth()
{
    return mediaVideoWidth;
}

void MediaPlayerPrivateMock::stateChanged(State oldState, State newState)
{
    assert(oldState == m_state);

    m_state = newState;

    switch (m_state) {
    case Idle:
        reset();
        break;
    case Initialized:
        assert(oldState == Idle);
        break;
    case Preparing:
        post([=] { onPrepared(); }, mediaPrepareLatency);
        break;
    case Prepared:
        assert(oldState == Preparing);
        break;
    case Started:
        onUpdatePlayback();
        onFrameAvailable();
        break;
    case Paused:
        break;
    case Stopped:
        break;
    case PlaybackCompleted:
        break;
    case End:
    case Error:
        break;
    default:
        assert(false);
        return;
    }
}

void MediaPlayerPrivateMock::copyFrame(GLTexture& texture)
{
    texture.image2D(mediaVideoWidth, mediaVideoHeight, GLTexture::Format::BGRA, 0);
}

void MediaPlayerPrivateMock::post(std::function<void ()> callback, int32_t delayMillis)
{
    synchronized (this) {
        if (m_suppressCallbacks)
            return;

        ++m_pendingCallbacks;
        Process::current().postDelayed([=] {
            synchronized (this) {
                --m_pendingCallbacks;
                notifyAll();
                if (m_suppressCallbacks)
                    return;
            }
            callback();
        }, std::chrono::milliseconds(delayMillis));
    }
}

void MediaPlayerPrivateMock::reset()
{
    synchronized (this) {
        m_suppressCallbacks = true;
        m_path = L"";
        m_duration = 0;
        m_buffering = 0;
        m_currentTime = 0;
        m_currentPosition = 0;
        while (m_pendingCallbacks > 0)
            wait();
        m_suppressCallbacks = false;
    }
}

void MediaPlayerPrivateMock::onBufferingUpdate(int32_t percent)
{
    m_buffering = std::max(0, std::min(percent, 100));
    callOnBufferingUpdateListener(m_buffering);
    if (m_buffering < 100)
        post([=] { onBufferingUpdate(m_buffering + mediaBufferingIncrement); }, mediaBufferingLatency);
}

void MediaPlayerPrivateMock::onCompletion()
{
    assert(m_state == Started);
    assert(m_currentTime > m_duration);

    m_currentTime = m_duration;
    callOnCompletionListener();
}

void MediaPlayerPrivateMock::onFrameAvailable()
{
    m_frameAvailableListener();
    if (m_state == Started)
        post([=] { onFrameAvailable(); }, mediaFrameAvailableLatency);
}

void MediaPlayerPrivateMock::onPrepared()
{
    m_duration = mediaDuration;
    callOnPreparedListener();
    onBufferingUpdate(mediaInitialBuffering);
    onVideoSizeChanged();
}

void MediaPlayerPrivateMock::onSeekComplete(int32_t msec)
{
    m_currentTime = msec;
    m_currentPosition = 0;
    callOnSeekCompleteListener();
}

void MediaPlayerPrivateMock::onVideoSizeChanged()
{
    callOnVideoSizeChangedListener(mediaVideoWidth, mediaVideoHeight);
}

void MediaPlayerPrivateMock::onUpdatePlayback()
{
    if (m_state != Started)
        return;

    m_currentTime += mediaFrameAvailableLatency;
    if (m_currentTime > m_duration)
        onCompletion();
    if (m_state == Started)
        post([=] { onUpdatePlayback(); }, mediaFrameAvailableLatency);
}

} // namespace graphics
} // namespace android
