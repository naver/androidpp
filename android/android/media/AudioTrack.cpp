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

#include "AudioTrack.h"

#include <android/media/AudioTrackPrivate.h>
#include <platforms/LogHelper.h>

#include <algorithm>

#include <assert>

namespace android {
namespace media {

static int32_t encodingBitsPerSample(int32_t encoding)
{
    switch (encoding) {
    case AudioFormat::ENCODING_PCM_16BIT:
        return 16;
    case AudioFormat::ENCODING_PCM_8BIT:
        return 8;
    case AudioFormat::ENCODING_PCM_FLOAT:
        return 32;
    default:
        assert(false);
        return 0;
    }
}

std::shared_ptr<AudioTrack> AudioTrack::create(int32_t streamType, int32_t sampleRateInHz, int32_t channelConfig, int32_t audioFormat, int32_t bufferSizeInBytes, int32_t mode)
{
    return std::shared_ptr<AudioTrack>(new AudioTrack(streamType, sampleRateInHz, channelConfig, audioFormat, bufferSizeInBytes, mode));
}

AudioTrack::AudioTrack(int32_t streamType, int32_t sampleRateInHz, int32_t channelConfig, int32_t audioFormat, int32_t bufferSizeInBytes, int32_t mode)
    : m_private(AudioTrackPrivate::create(*this))
    , m_playState(PLAYSTATE_STOPPED)
    , m_state(STATE_UNINITIALIZED)
    , m_streamType(streamType)
    , m_sampleRateInHz(sampleRateInHz)
    , m_channelConfig(channelConfig)
    , m_audioFormat(audioFormat)
    , m_bufferSizeInBytes(bufferSizeInBytes)
    , m_mode(mode)
{
    assert(m_streamType == AudioFormat::STREAM_MUSIC);

    m_state = m_private->initialize(m_streamType, m_sampleRateInHz, getChannelCount(), encodingBitsPerSample(m_audioFormat), m_bufferSizeInBytes, m_mode);
}

AudioTrack::~AudioTrack()
{
    release();
}

float AudioTrack::getMaxVolume()
{
    return AudioTrackPrivate::getMaxVolume();
}

float AudioTrack::getMinVolume()
{
    return AudioTrackPrivate::getMinVolume();
}

int32_t AudioTrack::getNativeOutputSampleRate(int32_t streamType)
{
    return AudioTrackPrivate::getNativeOutputSampleRate(streamType);
}

int32_t AudioTrack::getMinBufferSize(int32_t sampleRateInHz, int32_t channelConfig, int32_t audioFormat)
{
    int32_t channelCount = (channelConfig == AudioFormat::CHANNEL_OUT_MONO) ? 1 : 2;

    if (audioFormat != AudioFormat::ENCODING_PCM_16BIT &&
        audioFormat != AudioFormat::ENCODING_PCM_8BIT &&
        audioFormat != AudioFormat::ENCODING_PCM_FLOAT) {
        LOGE("getMinBufferSize(): Invalid audio format.");
        return ERROR_BAD_VALUE;
    }

    if (sampleRateInHz < 4000 || sampleRateInHz > 48000) {
        LOGE("getMinBufferSize(): %d Hz is not a supported sample rate.", sampleRateInHz);
        return ERROR_BAD_VALUE;
    }

    return AudioTrackPrivate::getMinBufferSize(sampleRateInHz, channelCount, encodingBitsPerSample(audioFormat));
}

int32_t AudioTrack::getAudioFormat()
{
    return m_audioFormat;
}

int32_t AudioTrack::getChannelConfiguration()
{
    return m_channelConfig;
}

int32_t AudioTrack::getChannelCount()
{
    return (m_channelConfig == AudioFormat::CHANNEL_OUT_MONO) ? 1 : 2;
}

int32_t AudioTrack::getPlayState()
{
    return m_playState;
}

int32_t AudioTrack::getSampleRate()
{
    return m_sampleRateInHz;
}

int32_t AudioTrack::getState()
{
    return m_state;
}

int32_t AudioTrack::getStreamType()
{
    return m_streamType;
}

int32_t AudioTrack::getPlaybackHeadPosition()
{
    return m_private->getPlaybackHeadPosition();
}

int32_t AudioTrack::getPlaybackRate()
{
    return m_private->getPlaybackRate();
}

int32_t AudioTrack::setPlaybackHeadPosition(int32_t positionInFrames)
{
    if (m_playState == PLAYSTATE_STOPPED || m_playState == PLAYSTATE_PAUSED)
        return m_private->setPlaybackHeadPosition(positionInFrames);

    return ERROR_INVALID_OPERATION;
}

int32_t AudioTrack::setPlaybackRate(int32_t sampleRateInHz)
{
    if (m_state != STATE_INITIALIZED)
        return ERROR_INVALID_OPERATION;

    if (sampleRateInHz <= 0)
        return ERROR_BAD_VALUE;

    return m_private->setPlaybackRate(sampleRateInHz);
}

int32_t AudioTrack::setVolume(float gain)
{
    if (m_state != STATE_INITIALIZED)
        return ERROR_INVALID_OPERATION;

    return m_private->setVolume(std::max(std::min(gain, getMaxVolume()), getMinVolume()));
}

int32_t AudioTrack::getNotificationMarkerPosition()
{
    return m_private->getNotificationMarkerPosition();
}

int32_t AudioTrack::getPositionNotificationPeriod()
{
    return m_private->getPositionNotificationPeriod();
}

int32_t AudioTrack::setNotificationMarkerPosition(int32_t markerInFrames)
{
    if (m_state != STATE_INITIALIZED)
        return ERROR_INVALID_OPERATION;

    return m_private->setNotificationMarkerPosition(markerInFrames);
}

int32_t AudioTrack::setPositionNotificationPeriod(int32_t periodInFrames)
{
    if (m_state != STATE_INITIALIZED)
        return ERROR_INVALID_OPERATION;

    return m_private->setPositionNotificationPeriod(periodInFrames);
}

void AudioTrack::setPlaybackPositionUpdateListener(OnPlaybackPositionUpdateListener listener)
{
    m_playbackPositionUpdateListener = std::move(listener);
}

void AudioTrack::setPlaybackPositionUpdateListener(OnPlaybackPositionUpdateListener listener, std::shared_ptr<os::Handler>& handler)
{
    m_playbackPositionUpdateListener = std::move(listener);
    m_playbackPositionUpdateHandler = handler;
}

void AudioTrack::play()
{
    assert(m_state == STATE_INITIALIZED);

    if (m_private->play())
        m_playState = PLAYSTATE_PLAYING;
}

void AudioTrack::pause()
{
    assert(m_state == STATE_INITIALIZED);

    if (m_private->pause())
        m_playState = PLAYSTATE_PAUSED;
}

void AudioTrack::stop()
{
    assert(m_state == STATE_INITIALIZED);

    if (m_private->stop())
        m_playState = PLAYSTATE_STOPPED;
}

void AudioTrack::release()
{
    stop();

    m_private->release();
    m_state = STATE_UNINITIALIZED;
}

void AudioTrack::flush()
{
    if (m_state == STATE_INITIALIZED)
        m_private->flush();
}

int32_t AudioTrack::write(float audioData[], int32_t offsetInFloats, int32_t sizeInFloats, int32_t writeMode)
{
    if (m_mode == MODE_STATIC &&
        m_state == STATE_NO_STATIC_DATA &&
        sizeInFloats > 0)
        m_state = STATE_INITIALIZED;

    if (m_state != STATE_INITIALIZED)
        return ERROR_INVALID_OPERATION;

    if (!audioData || offsetInFloats < 0 || sizeInFloats < 0)
        return ERROR_BAD_VALUE;

    return m_private->write(audioData, offsetInFloats, sizeInFloats, writeMode);
}

} // namespace media
} // namespace android
