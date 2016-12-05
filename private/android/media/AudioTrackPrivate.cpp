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

#include "AudioTrackPrivate.h"

#include <android/media/AudioTrack.h>

namespace android {
namespace media {

int32_t AudioTrackPrivate::initialize(int32_t streamType, int32_t sampleRateInHz, int32_t channelCount, int32_t bitsPerSample, int32_t bufferSizeInBytes, int32_t mode)
{
    int32_t bytesPerFrame = (channelCount * bitsPerSample) / 8;
    m_channelCount = channelCount;
    m_bytesPerFrame = bytesPerFrame;
    m_bufferSizeInFrames = bufferSizeInBytes / bytesPerFrame;
    return (mode == AudioTrack::MODE_STATIC) ? AudioTrack::STATE_NO_STATIC_DATA : AudioTrack::STATE_INITIALIZED;
}

int32_t AudioTrackPrivate::getNotificationMarkerPosition()
{
    return 0;
}

int32_t AudioTrackPrivate::getPositionNotificationPeriod()
{
    return m_positionNotificationPeriod;
}

int32_t AudioTrackPrivate::setNotificationMarkerPosition(int32_t markerInFrames)
{
    return 0;
}

int32_t AudioTrackPrivate::setPositionNotificationPeriod(int32_t periodInFrames)
{
    m_positionNotificationPeriod = periodInFrames;
    return AudioTrack::SUCCESS;
}

int32_t AudioTrackPrivate::write(float audioData[], int32_t offsetInFloats, int32_t sizeInFloats, int32_t)
{
    const char* dataBegin = reinterpret_cast<char*>(audioData + offsetInFloats);
    const char* dataEnd = reinterpret_cast<char*>(audioData + sizeInFloats);

    m_audioData.insert(m_audioData.end(), dataBegin, dataEnd);

    return AudioTrack::SUCCESS;
}

void AudioTrackPrivate::callOnMarkerReachedListener()
{
}

void AudioTrackPrivate::callOnPeriodicNotificationListener()
{
    if (m_positionNotificationPeriod == 0)
        return;

    m_track.m_playbackPositionUpdateListener.onPeriodicNotification(m_track);
}

} // namespace media
} // namespace android
