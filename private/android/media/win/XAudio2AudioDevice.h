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

#include <platforms/win/COMPtr.h>

#include <XAudio2.h>
#include <stdint.h>

#undef ERROR
#undef ERROR_INVALID_OPERATION

namespace android {
namespace media {

class XAudio2AudioDevice final : public IXAudio2EngineCallback {
public:  
    static XAudio2AudioDevice& shared();
    ~XAudio2AudioDevice();

    float getMaximumMasterVolume();
    float getMinimumMasterVolume();
    int32_t getMasterInputChannels();
    int32_t getMasterInputSampleRate();

    IXAudio2* device() const { return m_device; }

private:
    XAudio2AudioDevice();

    void initialize();
    void teardown();

    // IXAudio2EngineCallback
    void STDMETHODCALLTYPE OnProcessingPassStart() override;
    void STDMETHODCALLTYPE OnProcessingPassEnd() override;
    void STDMETHODCALLTYPE OnCriticalError(HRESULT error) override;

    IXAudio2* m_device;
    IXAudio2MasteringVoice* m_master;
    XAUDIO2_VOICE_DETAILS m_voiceDetails;
};

} // namespace graphics
} // namespace android
