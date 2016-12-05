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

#include "DataSourceProviderByteStream.h"

#if (WINVER >= _WIN32_WINNT_WIN7)

#include <platforms/win/Assertions.h>
#include <platforms/win/SoftLinking.h>

#include <algorithm>

#include <assert>

SOFT_LINK_LIBRARY(Mfplat);
SOFT_LINK_OPTIONAL(Mfplat, MFCreateAsyncResult, HRESULT, STDAPICALLTYPE, (IUnknown*, IMFAsyncCallback*, IUnknown*, IMFAsyncResult**));

namespace android {
namespace media {
namespace appkit {

DataSourceProviderByteStream::DataSourceProviderByteStream(std::function<int64_t (AudioFilePrivateMediaFoundationWin*, void*, int64_t, int64_t)> readCallback, AudioFilePrivateMediaFoundationWin* client)
    : m_client(client)
    , m_currentPosition(0)
    , m_farthestPosition(0)
    , m_endPosition(static_cast<QWORD>(-1))
    , m_readBytes(0)
    , m_reachedEndOfStream(FALSE)
    , m_readCallback(std::move(readCallback))
{
}

DataSourceProviderByteStream::~DataSourceProviderByteStream()
{
}

// {AD4C1B00-4BF7-422F-9175-756693D9130D}
static const GUID CLSID_IMFByteStream = 
{ 0xad4c1b00, 0x4bf7, 0x422f, { 0x91, 0x75, 0x75, 0x66, 0x93, 0xd9, 0x13, 0x0d } };

HRESULT STDMETHODCALLTYPE DataSourceProviderByteStream::QueryInterface(REFIID riid,
    _COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)
{
    *ppvObject = 0;
    if (IsEqualGUID(riid, CLSID_DataSourceProviderByteStream))
        *ppvObject = this;
    else if (IsEqualGUID(riid, IID_IUnknown))
        *ppvObject = static_cast<IMFByteStream*>(this);
    else if (IsEqualGUID(riid, CLSID_IMFByteStream))
        *ppvObject = static_cast<IMFByteStream*>(this);
    else
        return E_NOINTERFACE;

    AddRef();
    return S_OK;
}

ULONG STDMETHODCALLTYPE DataSourceProviderByteStream::AddRef(void)
{
    return ++m_refCount;
}

ULONG STDMETHODCALLTYPE DataSourceProviderByteStream::Release(void)
{
    ULONG newRef = --m_refCount;
    if (!newRef) {
        delete(this);
    }

    return newRef;
}

HRESULT STDMETHODCALLTYPE DataSourceProviderByteStream::GetCapabilities(
    __RPC__out DWORD *pdwCapabilities)
{
    *pdwCapabilities = MFBYTESTREAM_IS_READABLE | MFBYTESTREAM_IS_SEEKABLE;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE DataSourceProviderByteStream::GetLength(
    __RPC__out QWORD *pqwLength)
{
    *pqwLength = m_farthestPosition;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE DataSourceProviderByteStream::SetLength(QWORD qwLength)
{
    m_reachedEndOfStream = TRUE;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE DataSourceProviderByteStream::GetCurrentPosition(
    __RPC__out QWORD *pqwPosition)
{
    *pqwPosition = m_currentPosition;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE DataSourceProviderByteStream::SetCurrentPosition(QWORD qwPosition)
{
    m_currentPosition = qwPosition;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE DataSourceProviderByteStream::IsEndOfStream(
    __RPC__out BOOL *pfEndOfStream)
{
    *pfEndOfStream = m_reachedEndOfStream;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE DataSourceProviderByteStream::Read(
    __RPC__out_ecount_full(cb) BYTE *pb,
    ULONG cb,
    __RPC__out ULONG *pcbRead)
{
    return S_OK;
}

HRESULT STDMETHODCALLTYPE DataSourceProviderByteStream::BeginRead(
    _Out_writes_bytes_(cb)  BYTE *pb,
    ULONG cb,
    IMFAsyncCallback *pCallback,
    IUnknown *punkState)
{
    if (!MFCreateAsyncResultPtr())
        return E_FAIL;

    HRESULT hr = S_OK;

    int64_t bytesRead = m_readCallback(m_client, pb, m_currentPosition, cb);
    m_currentPosition += bytesRead;
    m_readBytes = bytesRead;

    m_farthestPosition = std::max(m_currentPosition, m_farthestPosition);
    if (m_reachedEndOfStream)
        m_endPosition = m_farthestPosition;

    COMPtr<IMFAsyncResult> result;
    hr = MFCreateAsyncResultPtr()(this, pCallback, punkState, &result);
    CHECK_HRESULT(hr, E_FAIL);

    result->SetStatus(S_OK);
    pCallback->Invoke(result.GetInterfacePtr());
    return S_OK;
}

HRESULT STDMETHODCALLTYPE DataSourceProviderByteStream::EndRead(
    IMFAsyncResult *pResult,
    _Out_  ULONG *pcbRead)
{
    *pcbRead = m_readBytes;
    m_readBytes = 0;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE DataSourceProviderByteStream::Write(
    __RPC__in_ecount_full(cb) const BYTE *pb,
    ULONG cb,
    __RPC__out ULONG *pcbWritten)
{
    return S_OK;
}

HRESULT STDMETHODCALLTYPE DataSourceProviderByteStream::BeginWrite(
    _In_reads_bytes_(cb)  const BYTE *pb,
    ULONG cb,
    IMFAsyncCallback *pCallback,
    IUnknown *punkState)
{
    return S_OK;
}

HRESULT STDMETHODCALLTYPE DataSourceProviderByteStream::EndWrite(
    IMFAsyncResult *pResult,
    _Out_  ULONG *pcbWritten)
{
    return S_OK;
}

HRESULT STDMETHODCALLTYPE DataSourceProviderByteStream::Seek(
    MFBYTESTREAM_SEEK_ORIGIN SeekOrigin,
    LONGLONG llSeekOffset,
    DWORD dwSeekFlags,
    __RPC__out QWORD *pqwCurrentPosition)
{
    switch (SeekOrigin) {
    case msoBegin:
        m_currentPosition = llSeekOffset;
        break;
    case msoCurrent:
        m_currentPosition += llSeekOffset;
        break;
    default:
        assert(false);
        return E_INVALIDARG;
    }

    *pqwCurrentPosition = m_currentPosition;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE DataSourceProviderByteStream::Flush(void)
{
    return S_OK;
}

HRESULT STDMETHODCALLTYPE DataSourceProviderByteStream::Close(void)
{
    m_reachedEndOfStream = FALSE;
    return S_OK;
}

} // namespace appkit
} // namespace media
} // namespace android

#endif
