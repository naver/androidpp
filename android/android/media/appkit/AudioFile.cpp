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

#include "AudioFile.h"

#include <android/media/appkit/AudioFilePrivate.h>

#include <assert>

namespace android {
namespace media {
namespace appkit {

std::shared_ptr<AudioFile> AudioFile::create()
{
    return std::shared_ptr<AudioFile>(new AudioFile);
}

AudioFile::AudioFile()
    : m_private(AudioFilePrivate::create(*this))
    , m_state(AudioFilePrivate::Idle)
{
}

AudioFile::~AudioFile()
{
    reset();
}

void AudioFile::setDataSource(String& path)
{
    assert(m_state == AudioFilePrivate::Idle);

    m_private->setDataSource(path);
    stateChanged(AudioFilePrivate::Initialized);
}

void AudioFile::setDataSource(DataSourceProvider provider)
{
    assert(m_state == AudioFilePrivate::Idle);

    m_private->setDataSource(provider);
    stateChanged(AudioFilePrivate::Initialized);
}

void AudioFile::setOnCompletionListener(OnCompletionListener listener)
{
    assert(m_state == AudioFilePrivate::Initialized || m_state == AudioFilePrivate::Preparing || m_state == AudioFilePrivate::Prepared);

    m_completionListener = std::move(listener);
}

void AudioFile::setOnErrorListener(OnErrorListener listener)
{
    assert(m_state == AudioFilePrivate::Initialized || m_state == AudioFilePrivate::Preparing || m_state == AudioFilePrivate::Prepared);

    m_errorListener = std::move(listener);
}

void AudioFile::setOnSampleAvailableListener(OnSampleAvailableListener listener)
{
    assert(m_state == AudioFilePrivate::Initialized || m_state == AudioFilePrivate::Preparing || m_state == AudioFilePrivate::Prepared);

    m_sampleAvailableListener = std::move(listener);
}

void AudioFile::prepare()
{
    assert(m_state == AudioFilePrivate::Initialized || m_state == AudioFilePrivate::Stopped);

    stateChanged(AudioFilePrivate::Preparing);
}

std::shared_ptr<MediaFormat> AudioFile::getOutputFormat()
{
    assert(m_state == AudioFilePrivate::Prepared || m_state == AudioFilePrivate::Started || m_state == AudioFilePrivate::Stopped || m_state == AudioFilePrivate::Ready);

    return m_private->getOutputFormat();
}

int64_t AudioFile::writeInputData(const int8_t data[], int64_t bytesCount)
{
    assert(m_state == AudioFilePrivate::Initialized || m_state == AudioFilePrivate::Preparing || m_state == AudioFilePrivate::Prepared || m_state == AudioFilePrivate::Started || m_state == AudioFilePrivate::Stopped || m_state == AudioFilePrivate::Ready);

    return m_private->writeInputData(data, bytesCount);
}

void AudioFile::start()
{
    assert(m_state == AudioFilePrivate::Prepared || m_state == AudioFilePrivate::Started || m_state == AudioFilePrivate::Stopped || m_state == AudioFilePrivate::Ready);

    stateChanged(AudioFilePrivate::Started);
}

void AudioFile::stop()
{
    assert(m_state == AudioFilePrivate::Prepared || m_state == AudioFilePrivate::Started || m_state == AudioFilePrivate::Stopped || m_state == AudioFilePrivate::Ready);

    stateChanged(AudioFilePrivate::Stopped);
}

void AudioFile::signalEndOfInputStream()
{
    assert(m_state == AudioFilePrivate::Preparing || m_state == AudioFilePrivate::Prepared || m_state == AudioFilePrivate::Started || m_state == AudioFilePrivate::Stopped || m_state == AudioFilePrivate::Ready);

    m_private->signalEndOfInputStream();
}

void AudioFile::reset()
{
    stateChanged(AudioFilePrivate::Idle);
}

static AudioFilePrivate::State toState(int32_t state)
{
    switch (state) {
    case AudioFilePrivate::Unknown:
    case AudioFilePrivate::Idle:
    case AudioFilePrivate::Initialized:
    case AudioFilePrivate::Preparing:
    case AudioFilePrivate::Prepared:
    case AudioFilePrivate::Started:
    case AudioFilePrivate::Stopped:
    case AudioFilePrivate::Ready:
    case AudioFilePrivate::End:
    case AudioFilePrivate::Error:
        return static_cast<AudioFilePrivate::State>(state);
    default:
        assert(false);
        break;
    };
    return AudioFilePrivate::Error;
}

void AudioFile::stateChanged(int32_t newState)
{
    AudioFilePrivate::State oldState = toState(m_state);
    m_state = newState;

    m_private->stateChanged(oldState, toState(newState));
}

} // namespace appkit
} // namespace media
} // namespace android
