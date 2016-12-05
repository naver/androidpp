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

#include "AudioFilePrivate.h"

namespace android {
namespace media {
namespace appkit {

void AudioFilePrivate::callOnCompletionListener()
{
    m_audio.stateChanged(End);
    m_audio.m_completionListener(m_audio);
}

void AudioFilePrivate::callOnErrorListener(int32_t error)
{
    m_audio.stateChanged(Error);
    m_audio.m_errorListener(m_audio, error);
}

void AudioFilePrivate::callOnSampleAvailableListener(const float samples[], int64_t samplesCount)
{
    m_audio.m_sampleAvailableListener(m_audio, samples, samplesCount);
}

void AudioFilePrivate::notifyPrepared()
{
    m_audio.stateChanged(Prepared);
}

void AudioFilePrivate::notifyReady()
{
    m_audio.stateChanged(Ready);
}

} // namespace appkit
} // namespace media
} // namespace android
