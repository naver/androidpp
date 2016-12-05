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

#pragma once

#include <java/lang.h>
#include <platforms/Functional.h>

namespace android {
namespace media {

class AudioTrackPrivate : public Object {
    friend class AudioTrack;
public:  
    static std::unique_ptr<AudioTrackPrivate> create(AudioTrack&);
    virtual ~AudioTrackPrivate() = default;

    static float getMaxVolume();
    static float getMinVolume();
    static int32_t getNativeOutputSampleRate(int32_t streamType);
    static int32_t getMinBufferSize(int32_t sampleRateInHz, int32_t channelConfig, int32_t audioFormat);

    virtual int32_t initialize(int32_t streamType, int32_t sampleRateInHz, int32_t channelCount, int32_t bitsPerSample, int32_t bufferSizeInBytes, int32_t mode);

    virtual int32_t getPlaybackHeadPosition() = 0;
    virtual int32_t getPlaybackRate() = 0;
    virtual int32_t setPlaybackHeadPosition(int32_t positionInFrames) = 0;
    virtual int32_t setPlaybackRate(int32_t sampleRateInHz) = 0;
    virtual int32_t setVolume(float gain) = 0;

    virtual int32_t getNotificationMarkerPosition();
    virtual int32_t getPositionNotificationPeriod();
    virtual int32_t setNotificationMarkerPosition(int32_t markerInFrames);
    virtual int32_t setPositionNotificationPeriod(int32_t periodInFrames);

    virtual bool play() = 0;
    virtual bool pause() = 0;
    virtual bool stop() = 0;
    virtual void release() = 0;
    virtual void flush() = 0;

    virtual int32_t write(float audioData[], int32_t offsetInFloats, int32_t sizeInFloats, int32_t writeMode);

protected:
    AudioTrackPrivate(AudioTrack& track)
        : m_track(track)
        , m_channelCount(0)
        , m_bytesPerFrame(0)
        , m_bufferSizeInFrames(0)
        , m_positionNotificationPeriod(0)
    {
    }

    size_t submitSizeInFrames() const { return (m_positionNotificationPeriod != 0) ? m_positionNotificationPeriod : m_bufferSizeInFrames / 2; }
    size_t submitSizeInBytes() const { return submitSizeInFrames() * m_bytesPerFrame; }

    void callOnMarkerReachedListener();
    void callOnPeriodicNotificationListener();

    std::vector<char> m_audioData;

private:
    media::AudioTrack& m_track;
    int32_t m_channelCount;
    int32_t m_bytesPerFrame;
    int32_t m_bufferSizeInFrames;
    int32_t m_positionNotificationPeriod;
};

} // namespace media
} // namespace android
