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

#include <platforms/Functional.h>
#include <platforms/win/COMPtr.h>

#if (WINVER >= _WIN32_WINNT_WIN7)

#include <mfobjects.h>

// {2CD5DA18-051A-4094-981D-1277E38BA3D0}
static const GUID CLSID_DataSourceProviderByteStream = 
{ 0x2cd5da18, 0x51a, 0x4094, { 0x98, 0x1d, 0x12, 0x77, 0xe3, 0x8b, 0xa3, 0xd0 } };

namespace android {
namespace media {
namespace appkit {

class AudioFilePrivateMediaFoundationWin;

class DataSourceProviderByteStream final : public IMFByteStream {
public:
    DataSourceProviderByteStream(std::function<int64_t (AudioFilePrivateMediaFoundationWin*, void*, int64_t, int64_t)>, AudioFilePrivateMediaFoundationWin*);
    ~DataSourceProviderByteStream();

    // IUnknown

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
        _COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject) override;

    ULONG STDMETHODCALLTYPE AddRef(void) override;

    ULONG STDMETHODCALLTYPE Release(void) override;

    // IMFByteStream

    HRESULT STDMETHODCALLTYPE GetCapabilities(
        __RPC__out DWORD *pdwCapabilities) override;

    HRESULT STDMETHODCALLTYPE GetLength(
        __RPC__out QWORD *pqwLength) override;

    HRESULT STDMETHODCALLTYPE SetLength(QWORD qwLength) override;

    HRESULT STDMETHODCALLTYPE GetCurrentPosition(
        __RPC__out QWORD *pqwPosition) override;

    HRESULT STDMETHODCALLTYPE SetCurrentPosition(QWORD qwPosition) override;

    HRESULT STDMETHODCALLTYPE IsEndOfStream(
        __RPC__out BOOL *pfEndOfStream) override;

    HRESULT STDMETHODCALLTYPE Read(
        __RPC__out_ecount_full(cb) BYTE *pb,
        ULONG cb,
        __RPC__out ULONG *pcbRead) override;

    HRESULT STDMETHODCALLTYPE BeginRead(
        _Out_writes_bytes_(cb)  BYTE *pb,
        ULONG cb,
        IMFAsyncCallback *pCallback,
        IUnknown *punkState) override;

    HRESULT STDMETHODCALLTYPE EndRead(
        IMFAsyncResult *pResult,
        _Out_  ULONG *pcbRead) override;

    HRESULT STDMETHODCALLTYPE Write(
        __RPC__in_ecount_full(cb) const BYTE *pb,
        ULONG cb,
        __RPC__out ULONG *pcbWritten) override;

    HRESULT STDMETHODCALLTYPE BeginWrite(
        _In_reads_bytes_(cb)  const BYTE *pb,
        ULONG cb,
        IMFAsyncCallback *pCallback,
        IUnknown *punkState) override;

    HRESULT STDMETHODCALLTYPE EndWrite(
        IMFAsyncResult *pResult,
        _Out_  ULONG *pcbWritten) override;

    HRESULT STDMETHODCALLTYPE Seek(
        MFBYTESTREAM_SEEK_ORIGIN SeekOrigin,
        LONGLONG llSeekOffset,
        DWORD dwSeekFlags,
        __RPC__out QWORD *pqwCurrentPosition) override;

    HRESULT STDMETHODCALLTYPE Flush(void) override;

    HRESULT STDMETHODCALLTYPE Close(void) override;

private:
    ULONG m_refCount;

    AudioFilePrivateMediaFoundationWin* m_client;
    QWORD m_currentPosition;
    QWORD m_farthestPosition;
    QWORD m_endPosition;
    int64_t m_readBytes;
    BOOL m_reachedEndOfStream;
    std::function<int64_t (AudioFilePrivateMediaFoundationWin*, void*, int64_t, int64_t)> m_readCallback;
};

} // namespace appkit
} // namespace media
} // namespace android

#endif
