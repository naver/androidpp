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

#include "AudioTrackPrivate.h"
#include "XAudio2AudioDevice.h"

#include <atomic>
#include <pthread.h>

namespace android {
namespace media {

class AudioTrackPrivateXAudio2Win final : public AudioTrackPrivate
    , public IXAudio2VoiceCallback {
public:  
    AudioTrackPrivateXAudio2Win(AudioTrack&);
    ~AudioTrackPrivateXAudio2Win();

    int32_t initialize(int32_t streamType, int32_t sampleRateInHz, int32_t channelCount, int32_t bitsPerSample, int32_t bufferSizeInBytes, int32_t mode) override;

    int32_t getPlaybackHeadPosition() override;
    int32_t getPlaybackRate() override;
    int32_t setPlaybackHeadPosition(int32_t positionInFrames) override;
    int32_t setPlaybackRate(int32_t sampleRateInHz) override;
    int32_t setVolume(float gain) override;

    bool play() override;
    bool pause() override;
    bool stop() override;
    void release() override;
    void flush() override;

private:
    bool submitSourceBuffer();

    // IXAudio2VoiceCallback
    void STDMETHODCALLTYPE OnVoiceProcessingPassStart(UINT32 bytesRequired) override;
    void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() override;
    void STDMETHODCALLTYPE OnStreamEnd() override;
    void STDMETHODCALLTYPE OnBufferStart(void* context) override;
    void STDMETHODCALLTYPE OnBufferEnd(void* context) override;
    void STDMETHODCALLTYPE OnLoopEnd(void* context) override;
    void STDMETHODCALLTYPE OnVoiceError(void* context, HRESULT error) override;

    IXAudio2SourceVoice* m_source;
    bool m_isPlaying;
    bool m_paused;
    bool m_initializedWorkerThread;
    std::atomic<bool> m_deletionHasBegun;
    pthread_main_np_t m_mainThreadIdentifier;
};

} // namespace graphics
} // namespace android
