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

#include <android/media/MediaFormat.h>
#include <android/media/appkit/AudioFile.h>

namespace android {
namespace media {
namespace appkit {

class AudioFilePrivate : public Object {
    friend class AudioFile;
public:  
    static std::unique_ptr<AudioFilePrivate> create(AudioFile&);
    virtual ~AudioFilePrivate() = default;

    enum State {
        Unknown,
        Idle,
        Initialized,
        Preparing,
        Prepared,
        Started,
        Stopped,
        Ready,
        End,
        Error,
    };

    enum ErrorCode {
        NoError,
        UnknownError,
        UnspecifiedError,
        UnsupportedFileTypeError,
        UnsupportedDataFormatError,
        PermissionsError,
        InvalidFileError,
    };

    typedef AudioFile::DataSourceProvider DataSourceProvider;

    virtual void setDataSource(const String&) = 0;
    virtual void setDataSource(DataSourceProvider) = 0;

    virtual std::shared_ptr<MediaFormat> getOutputFormat() = 0;

    virtual int64_t writeInputData(const int8_t data[], int64_t bytesCount) = 0;

    virtual void signalEndOfInputStream() = 0;

    // Called by AudioFile whenever its state is being changed.
    virtual void stateChanged(State oldState, State newState) = 0;

protected:
    AudioFilePrivate(AudioFile& audio)
        : m_audio(audio)
    {
    }

    void callOnCompletionListener();
    void callOnErrorListener(int32_t error);
    void callOnSampleAvailableListener(const float samples[], int64_t samplesCount);

    void notifyPrepared();
    void notifyReady();

    AudioFile& m_audio;
};

} // namespace appkit
} // namespace media
} // namespace android
