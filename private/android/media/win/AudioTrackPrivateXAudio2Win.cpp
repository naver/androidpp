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

#include "AudioTrackPrivateXAudio2Win.h"

#include "XAudio2AudioDevice.h"
#include <android/media/AudioTrack.h>

#include <platforms/win/Assertions.h>

#include <algorithm>
#include <iterator>
#include <utility>

namespace android {
namespace media {

std::unique_ptr<AudioTrackPrivate> AudioTrackPrivate::create(AudioTrack& track)
{
    return std::unique_ptr<AudioTrackPrivate>(new AudioTrackPrivateXAudio2Win(track));
}

float AudioTrackPrivate::getMaxVolume()
{
    return XAudio2AudioDevice::shared().getMaximumMasterVolume();
}

float AudioTrackPrivate::getMinVolume()
{
    return XAudio2AudioDevice::shared().getMinimumMasterVolume();
}

int32_t AudioTrackPrivate::getNativeOutputSampleRate(int32_t streamType)
{
    return XAudio2AudioDevice::shared().getMasterInputSampleRate();
}

int32_t AudioTrackPrivate::getMinBufferSize(int32_t sampleRateInHz, int32_t channelCount, int32_t bitsPerSample)
{
    int32_t samplesPerSec = sampleRateInHz * channelCount * (bitsPerSample / 8);
    return samplesPerSec * 2 / 100;
}

static WAVEFORMATEX waveFormat(int32_t sampleRateInHz, int32_t channelCount, int32_t bitsPerSample)
{
    WAVEFORMATEX wave = { 0, };
    wave.wFormatTag = (bitsPerSample == 32) ? WAVE_FORMAT_IEEE_FLOAT : WAVE_FORMAT_PCM;
    wave.nChannels = channelCount;
    wave.nSamplesPerSec = sampleRateInHz;
    wave.nBlockAlign = (channelCount * bitsPerSample) / 8;
    wave.nAvgBytesPerSec = sampleRateInHz * wave.nBlockAlign;
    wave.wBitsPerSample = bitsPerSample;
    wave.cbSize = sizeof(WAVEFORMATEX);
    return wave;
}

AudioTrackPrivateXAudio2Win::AudioTrackPrivateXAudio2Win(AudioTrack& track)
    : AudioTrackPrivate(track)
    , m_source(nullptr)
    , m_isPlaying(false)
    , m_paused(false)
    , m_deletionHasBegun(false)
    , m_initializedWorkerThread(false)
    , m_mainThreadIdentifier(pthread_get_main_np())
{
}

AudioTrackPrivateXAudio2Win::~AudioTrackPrivateXAudio2Win()
{
    m_deletionHasBegun = true;

    if (m_source) {
        m_source->DestroyVoice();
        m_source = NULL;
    }
}

int32_t AudioTrackPrivateXAudio2Win::initialize(int32_t streamType, int32_t sampleRateInHz, int32_t channelCount, int32_t bitsPerSample, int32_t bufferSizeInBytes, int32_t mode)
{
    int32_t result = AudioTrackPrivate::initialize(streamType, sampleRateInHz, channelCount, bitsPerSample, bufferSizeInBytes, mode);

    HRESULT hr = S_OK;

    IXAudio2* device = XAudio2AudioDevice::shared().device();
    if (device)
        return AudioTrack::STATE_UNINITIALIZED;

    WAVEFORMATEX wave = waveFormat(sampleRateInHz, channelCount, bitsPerSample);
    hr = device->CreateSourceVoice(&m_source, &wave, 0, XAUDIO2_DEFAULT_FREQ_RATIO, this, NULL, NULL);
    CHECK_HRESULT(hr, AudioTrack::ERROR);

    return result;
}

int32_t AudioTrackPrivateXAudio2Win::getPlaybackHeadPosition()
{
    return 0;
}

int32_t AudioTrackPrivateXAudio2Win::getPlaybackRate()
{
    return 0;
}

int32_t AudioTrackPrivateXAudio2Win::setPlaybackHeadPosition(int32_t positionInFrames)
{
    return 0;
}

int32_t AudioTrackPrivateXAudio2Win::setPlaybackRate(int32_t sampleRateInHz)
{
    return 0;
}

int32_t AudioTrackPrivateXAudio2Win::setVolume(float gain)
{
    return 0;
}

bool AudioTrackPrivateXAudio2Win::play()
{
    if (m_isPlaying)
        return true;

    if (!submitSourceBuffer())
        return false;

    if (!m_paused)
        if (!submitSourceBuffer())
            return false;

    HRESULT hr = S_OK;

    hr = m_source->Start();
    CHECK_HRESULT(hr, false);

    m_isPlaying = true;
    m_paused = false;

    return true;
}

bool AudioTrackPrivateXAudio2Win::pause()
{
    if (!m_isPlaying)
        return true;

    HRESULT hr = S_OK;

    hr = m_source->Stop();
    CHECK_HRESULT(hr, false);

    m_isPlaying = false;
    m_paused = true;

    return true;
}

bool AudioTrackPrivateXAudio2Win::stop()
{
    if (!m_isPlaying && !m_paused)
        return true;

    HRESULT hr = S_OK;

    hr = m_source->Stop();
    CHECK_HRESULT(hr, false);

    hr = m_source->FlushSourceBuffers();
    CHECK_HRESULT(hr, false);

    m_isPlaying = false;
    m_paused = false;

    return 0;
}

void AudioTrackPrivateXAudio2Win::release()
{
}

void AudioTrackPrivateXAudio2Win::flush()
{
}

struct AudioDataContext {
    std::vector<char> audioData;
};

bool AudioTrackPrivateXAudio2Win::submitSourceBuffer()
{
    if (m_audioData.empty())
        return false;

    const size_t submitSize = std::min(submitSizeInBytes(), m_audioData.size());

    AudioDataContext* context = new AudioDataContext;
    auto srcEnd = std::next(m_audioData.begin(), submitSize);

    std::move(m_audioData.begin(), srcEnd, std::back_inserter(context->audioData));
    m_audioData.erase(m_audioData.begin(), srcEnd);

    XAUDIO2_BUFFER audioBuffer = { 0, };
    audioBuffer.AudioBytes = submitSize;
    audioBuffer.pAudioData = (BYTE*)context->audioData.data();
    audioBuffer.pContext = context;

    HRESULT hr = S_OK;

    hr = m_source->SubmitSourceBuffer(&audioBuffer);
    CHECK_HRESULT(hr, false);

    return true;
}

// IXAudio2VoiceCallback
void STDMETHODCALLTYPE AudioTrackPrivateXAudio2Win::OnVoiceProcessingPassStart(UINT32 bytesRequired)
{
}

void STDMETHODCALLTYPE AudioTrackPrivateXAudio2Win::OnVoiceProcessingPassEnd()
{
}

void STDMETHODCALLTYPE AudioTrackPrivateXAudio2Win::OnStreamEnd()
{
}

void STDMETHODCALLTYPE AudioTrackPrivateXAudio2Win::OnBufferStart(void* context)
{
}

void STDMETHODCALLTYPE AudioTrackPrivateXAudio2Win::OnBufferEnd(void* context)
{
    delete reinterpret_cast<AudioDataContext*>(context);

    if (m_deletionHasBegun)
        return;

    if (!m_initializedWorkerThread) {
        pthread_init_current_np(m_mainThreadIdentifier);
        m_initializedWorkerThread = true;
    }

    AudioTrackPrivate::callOnPeriodicNotificationListener();

    if (!submitSourceBuffer()) {
        LOGE("Error at AudioTrackPrivateXAudio2Win.OnBufferEnd()");
    }
}

void STDMETHODCALLTYPE AudioTrackPrivateXAudio2Win::OnLoopEnd(void* context)
{
}

void STDMETHODCALLTYPE AudioTrackPrivateXAudio2Win::OnVoiceError(void* context, HRESULT error)
{
}

} // namespace graphics
} // namespace android
