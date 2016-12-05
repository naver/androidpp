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

#include "AudioFilePrivate.h"

#if (WINVER >= _WIN32_WINNT_WIN7)

#include <platforms/win/COMPtr.h>

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

struct IMFByteStream;
struct IMFSourceReader;

namespace android {
namespace media {
namespace appkit {

class AudioFilePrivateMediaFoundationWin final : public AudioFilePrivate
    , public IMFSourceReaderCallback {
public:  
    AudioFilePrivateMediaFoundationWin(AudioFile&);
    ~AudioFilePrivateMediaFoundationWin();

    void setDataSource(const String&) override;
    void setDataSource(DataSourceProvider) override;

    std::shared_ptr<MediaFormat> getOutputFormat() override;

    int64_t writeInputData(const int8_t data[], int64_t bytesCount) override;

    void signalEndOfInputStream() override;

    // Called by AudioFile whenever its state is being changed.
    void stateChanged(State oldState, State newState) override;

private:
    void reset();

    bool prepare();
    void start();

    static int64_t inputByteStreamReadCallback(AudioFilePrivateMediaFoundationWin*, void*, int64_t, int64_t);

    // IUnknown

    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
        _COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject) override;

    virtual ULONG STDMETHODCALLTYPE AddRef(void) override;

    virtual ULONG STDMETHODCALLTYPE Release(void) override;

    // IMFSourceReaderCallback

    virtual HRESULT STDMETHODCALLTYPE OnReadSample(_In_  HRESULT hrStatus,
        _In_  DWORD dwStreamIndex, _In_  DWORD dwStreamFlags, _In_  LONGLONG llTimestamp,
        _In_opt_  IMFSample *pSample) override;
        
    virtual HRESULT STDMETHODCALLTYPE OnFlush(_In_  DWORD dwStreamIndex) override;
        
    virtual HRESULT STDMETHODCALLTYPE OnEvent(_In_  DWORD dwStreamIndex,
        _In_  IMFMediaEvent *pEvent) override;
        
private:
    ULONG m_refCount;

    State m_state;
    String m_path;
    DataSourceProvider m_provider;

    bool m_requestedInputData;
    std::pair<void*, int64_t> m_inputData;

    COMPtr<IMFSourceReader> m_reader;
    COMPtr<IMFByteStream> m_stream;
    COMPtr<IMFMediaType> m_mediaType;
};

} // namespace appkit
} // namespace media
} // namespace android

#endif
