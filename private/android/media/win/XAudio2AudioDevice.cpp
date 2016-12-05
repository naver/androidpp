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

#include "XAudio2AudioDevice.h"

#include <platforms/win/Assertions.h>

namespace android {
namespace media {

XAudio2AudioDevice& XAudio2AudioDevice::shared()
{
    static XAudio2AudioDevice device;
    return device;
}

XAudio2AudioDevice::XAudio2AudioDevice()
    : m_device(NULL)
    , m_master(NULL)
{
    initialize();
}

XAudio2AudioDevice::~XAudio2AudioDevice()
{
    teardown();
}

float XAudio2AudioDevice::getMaximumMasterVolume()
{
    return XAUDIO2_MAX_VOLUME_LEVEL;
}

float XAudio2AudioDevice::getMinimumMasterVolume()
{
    return 0.0f;
}

int32_t XAudio2AudioDevice::getMasterInputChannels()
{
    return m_voiceDetails.InputChannels;
}

int32_t XAudio2AudioDevice::getMasterInputSampleRate()
{
    return m_voiceDetails.InputSampleRate;
}

void XAudio2AudioDevice::initialize()
{
    if (m_device)
        return;

    HRESULT hr = S_OK;

    // Initialize the COM library.
    hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    CHECK_HRESULT(hr);

    DWORD flags = 0;
#if (_WIN32_WINNT < 0x0602 /*_WIN32_WINNT_WIN8*/) && !defined(NDEBUG)
    flags |= XAUDIO2_DEBUG_ENGINE;
#endif
    hr = XAudio2Create(&m_device, flags);
    if (FAILED(hr) && flags)
        hr = XAudio2Create(&m_device, 0);
    CHECK_HRESULT(hr);
    hr = m_device->RegisterForCallbacks(this);
    CHECK_HRESULT(hr);
    hr = m_device->CreateMasteringVoice(&m_master, XAUDIO2_DEFAULT_CHANNELS, XAUDIO2_DEFAULT_SAMPLERATE);
    CHECK_HRESULT(hr);

    m_master->GetVoiceDetails(&m_voiceDetails);

    hr = m_device->StartEngine();
    CHECK_HRESULT(hr);
}

void XAudio2AudioDevice::teardown()
{
    if (!m_device)
        return;

    m_device->StopEngine();

    if (m_master) {
        m_master->DestroyVoice();
        m_master = NULL;
    }

    if (m_device) {
        m_device->Release();
        m_device = NULL;
    }

    CoUninitialize();
}

// IXAudio2EngineCallback
void STDMETHODCALLTYPE XAudio2AudioDevice::OnProcessingPassStart()
{
}

void STDMETHODCALLTYPE XAudio2AudioDevice::OnProcessingPassEnd()
{
}

void STDMETHODCALLTYPE XAudio2AudioDevice::OnCriticalError(HRESULT error)
{
}

} // namespace graphics
} // namespace android
