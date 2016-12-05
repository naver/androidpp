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

#include "AudioFilePrivateMediaFoundationWin.h"

#if (WINVER >= _WIN32_WINNT_WIN7)

#include <android/media/MediaFormat.h>
#include <android/media/appkit/AudioFile.h>
#include <android/media/win/DataSourceProviderByteStream.h>

#include <android/os/appkit/Process.h>
#include <platforms/StringConversion.h>
#include <platforms/win/SoftLinking.h>

SOFT_LINK_LIBRARY(Mfplat);
SOFT_LINK_OPTIONAL(Mfplat, MFStartup, HRESULT, STDAPICALLTYPE, (ULONG, DWORD));
SOFT_LINK_OPTIONAL(Mfplat, MFShutdown, HRESULT, STDAPICALLTYPE, ());
SOFT_LINK_OPTIONAL(Mfplat, MFCreateMediaType, HRESULT, STDAPICALLTYPE, (IMFMediaType**));

SOFT_LINK_LIBRARY(Mfreadwrite);
SOFT_LINK_OPTIONAL(Mfreadwrite, MFCreateSourceReaderFromURL, HRESULT, STDAPICALLTYPE, (LPCWSTR, IMFAttributes*, IMFSourceReader**));
SOFT_LINK_OPTIONAL(Mfreadwrite, MFCreateSourceReaderFromByteStream, HRESULT, STDAPICALLTYPE, (IMFByteStream*, IMFAttributes*, IMFSourceReader**));

namespace android {
namespace media {
namespace appkit {

std::unique_ptr<AudioFilePrivate> AudioFilePrivate::create(AudioFile& audio)
{
    return std::unique_ptr<AudioFilePrivate>(new AudioFilePrivateMediaFoundationWin(audio));
}

AudioFilePrivateMediaFoundationWin::AudioFilePrivateMediaFoundationWin(AudioFile& audio)
    : AudioFilePrivate(audio)
    , m_refCount(0)
    , m_state(Idle)
    , m_requestedInputData(false)
{
    if (!MFStartupPtr())
        return;

    HRESULT hr = S_OK;

    // Initialize the COM library.
    hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    CHECK_HRESULT(hr);
    // Intialize the Media Foundation platform.
    hr = MFStartupPtr()(MF_VERSION, MFSTARTUP_FULL);
    CHECK_HRESULT(hr);
}

AudioFilePrivateMediaFoundationWin::~AudioFilePrivateMediaFoundationWin()
{
    if (!MFShutdownPtr())
        return;

    reset();

    MFShutdownPtr()();
    CoUninitialize();
}

void AudioFilePrivateMediaFoundationWin::setDataSource(const String& path)
{
    m_path = path;
    m_provider = nullptr;
}

void AudioFilePrivateMediaFoundationWin::setDataSource(DataSourceProvider f)
{
    m_provider = std::move(f);
    m_path = L"";
}

std::shared_ptr<MediaFormat> AudioFilePrivateMediaFoundationWin::getOutputFormat()
{
    HRESULT hr = S_OK;

    UINT32 samplesPerSecond = 0;
    hr = m_mediaType->GetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, &samplesPerSecond);
    CHECK_HRESULT(hr, nullptr);

    UINT32 numChannels = 0;
    hr = m_mediaType->GetUINT32(MF_MT_AUDIO_NUM_CHANNELS, &numChannels);
    CHECK_HRESULT(hr, nullptr);

    return MediaFormat::createAudioFormat(MediaFormat::MIMETYPE_AUDIO_RAW, samplesPerSecond, numChannels);
}

int64_t AudioFilePrivateMediaFoundationWin::writeInputData(const int8_t data[], int64_t bytesCount)
{
    assert(m_requestedInputData);
    assert(m_inputData.first);

    memcpy(m_inputData.first, data, bytesCount);
    m_inputData.second = bytesCount;
    return bytesCount;
}

void AudioFilePrivateMediaFoundationWin::signalEndOfInputStream()
{
    m_stream->SetLength(0);
}

void AudioFilePrivateMediaFoundationWin::stateChanged(State oldState, State newState)
{
    assert(oldState == m_state);

    m_state = newState;

    switch (m_state) {
    case Idle:
        reset();
        break;
    case Initialized:
        assert(oldState == Idle);
        assert(m_path.size() > 0 || m_provider);
        break;
    case Preparing:
        if (!prepare())
            callOnErrorListener(UnknownError);
        break;
    case Prepared:
        assert(oldState == Preparing);
        break;
    case Started:
        start();
        break;
    case Stopped:
        break;
    case Ready:
        break;
    case End:
    case Error:
        break;
    default:
        assert(false);
        return;
    }
}

void AudioFilePrivateMediaFoundationWin::reset()
{
    assert(!m_requestedInputData);

    m_path = L"";
    m_provider = nullptr;

    m_stream.Detach();
    m_reader.Detach();
    m_mediaType.Detach();
}

bool AudioFilePrivateMediaFoundationWin::prepare()
{
    if (!MFCreateSourceReaderFromByteStreamPtr() || !MFCreateSourceReaderFromURLPtr() || !MFCreateMediaTypePtr())
        return false;

    HRESULT hr = S_OK;

    bool hasDataSourceProvider = static_cast<bool>(m_provider);
    if (hasDataSourceProvider) {
        m_stream = new DataSourceProviderByteStream(inputByteStreamReadCallback, this);
        hr = MFCreateSourceReaderFromByteStreamPtr()(m_stream.GetInterfacePtr(), NULL, &m_reader);
        CHECK_HRESULT(hr, false);
    } else {
        hr = MFCreateSourceReaderFromURLPtr()(m_path.c_str(), NULL, &m_reader);
        CHECK_HRESULT(hr, false);
    }

    assert(m_reader);

    COMPtr<IMFMediaType> partialType;

    // Create a partial media type that specifies uncompressed PCM audio.
    hr = MFCreateMediaTypePtr()(&partialType);
    CHECK_HRESULT(hr, false);
    hr = partialType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
    CHECK_HRESULT(hr, false);
    hr = partialType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_Float);
    CHECK_HRESULT(hr, false);

    // Set this type on the source reader. The source reader will
    // load the necessary decoder.
    hr = m_reader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, NULL, partialType.GetInterfacePtr());
    CHECK_HRESULT(hr, false);
    // Get the complete uncompressed format.
    hr = m_reader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, &m_mediaType);
    CHECK_HRESULT(hr, false);
    // Ensure the stream is selected.
    hr = m_reader->SetStreamSelection((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, TRUE);
    CHECK_HRESULT(hr, false);

    notifyPrepared();
    return true;
}

void AudioFilePrivateMediaFoundationWin::start()
{
    HRESULT hr = S_OK;

    COMPtr<IMFSample> sample;
    COMPtr<IMFMediaBuffer> buffer;

    BYTE* audioData = NULL;
    DWORD audioDataLength = 0;

    // Get audio samples from the source reader.
    while (true) {
        DWORD streamFlags = 0;
        LONGLONG timeStamp = 0;

        // Read the next sample.
        hr = m_reader->ReadSample((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, NULL,
            &streamFlags, &timeStamp, &sample);
        CHECK_HRESULT(hr);

        if (streamFlags & MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED) {
            LOGE("Type change - not supported by WAVE file format.\n");
            break;
        }

        if (streamFlags & MF_SOURCE_READERF_ENDOFSTREAM) {
            LOGD("End of input file.\n");
            break;
        }

        if (!sample) {
            LOGD("No sample\n");
            continue;
        }
        
        // Get a pointer to the audio data in the sample.
        hr = sample->ConvertToContiguousBuffer(&buffer);
        CHECK_HRESULT(hr);

        hr = buffer->Lock(&audioData, NULL, &audioDataLength);
        CHECK_HRESULT(hr);

        callOnSampleAvailableListener(reinterpret_cast<float*>(audioData), audioDataLength / sizeof(float));

        // Unlock the buffer.
        hr = buffer->Unlock();
        audioData = NULL;
        CHECK_HRESULT(hr);

        sample.Detach();
        buffer.Detach();
    }

    callOnCompletionListener();

    if (audioData)
        buffer->Unlock();
}

int64_t AudioFilePrivateMediaFoundationWin::inputByteStreamReadCallback(AudioFilePrivateMediaFoundationWin* thisObject, void* buffer, int64_t offset, int64_t length)
{
    thisObject->m_requestedInputData = true;
    thisObject->m_inputData.first = buffer;

    thisObject->m_provider(offset, length);

    thisObject->m_requestedInputData = false;
    thisObject->m_inputData.first = nullptr;

    return thisObject->m_inputData.second;
}

// IUnknown

// {DEEC8D99-FA1D-4D82-84C2-2C8969944867}
static const GUID CLSID_IMFSourceReaderCallback = 
{ 0xdeec8d99, 0xfa1d, 0x4d82, { 0x84, 0xc2, 0x2c, 0x89, 0x69, 0x94, 0x48, 0x67 } };

HRESULT STDMETHODCALLTYPE AudioFilePrivateMediaFoundationWin::QueryInterface(REFIID riid,
    _COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)
{
    *ppvObject = 0;
    if (IsEqualGUID(riid, IID_IUnknown))
        *ppvObject = static_cast<IMFSourceReaderCallback*>(this);
    else if (IsEqualGUID(riid, CLSID_IMFSourceReaderCallback))
        *ppvObject = static_cast<IMFSourceReaderCallback*>(this);
    else
        return E_NOINTERFACE;

    AddRef();
    return S_OK;
}

ULONG STDMETHODCALLTYPE AudioFilePrivateMediaFoundationWin::AddRef(void)
{
    return ++m_refCount;
}

ULONG STDMETHODCALLTYPE AudioFilePrivateMediaFoundationWin::Release(void)
{
    return --m_refCount;
}

// IMFSourceReaderCallback

HRESULT STDMETHODCALLTYPE AudioFilePrivateMediaFoundationWin::OnReadSample(_In_  HRESULT hrStatus,
    _In_  DWORD dwStreamIndex, _In_  DWORD dwStreamFlags, _In_  LONGLONG llTimestamp,
    _In_opt_  IMFSample *sample)
{
    return S_OK;
}
        
HRESULT STDMETHODCALLTYPE AudioFilePrivateMediaFoundationWin::OnFlush(_In_  DWORD dwStreamIndex)
{
    return S_OK;
}
        
HRESULT STDMETHODCALLTYPE AudioFilePrivateMediaFoundationWin::OnEvent(_In_  DWORD dwStreamIndex,
    _In_  IMFMediaEvent *pEvent)
{
    return S_OK;
}

} // namespace appkit
} // namespace media
} // namespace android

#endif
