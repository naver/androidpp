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

#include <platforms/Functional.h>

namespace android {
namespace media {
namespace appkit {

class AudioFilePrivate;

class AudioFile {
    friend class AudioFilePrivate;
public:
    static const int32_t NO_ERR = 0;
    static const int32_t UNKNOWN_ERROR = 1;
    static const int32_t UNSPECIFIED_ERROR = 2;
    static const int32_t UNSUPPORTED_FILE_TYPE_ERROR = 3;
    static const int32_t UNSUPPORTED_DATA_FORMAT_ERROR = 4;
    static const int32_t PERMISSIONS_ERROR = 5;
    static const int32_t INVALID_FILE_ERROR = 6;

    ANDROID_EXPORT static std::shared_ptr<AudioFile> create();
    ANDROID_EXPORT virtual ~AudioFile();

    typedef std::function<void (int64_t, int64_t)> DataSourceProvider;
    typedef std::function<void (AudioFile&)> OnCompletionListener;
    typedef std::function<void (AudioFile&, int32_t)> OnErrorListener;
    typedef std::function<void (AudioFile&, const float[], int64_t)> OnSampleAvailableListener;

    // Sets the data source (file-path or http/rtsp URL) to use.
    ANDROID_EXPORT virtual void setDataSource(String&);
    // Sets the data source (provider) to use.
    ANDROID_EXPORT virtual void setDataSource(DataSourceProvider);

    // Register a callback to be invoked when the audio file has finished processing.
    ANDROID_EXPORT virtual void setOnCompletionListener(OnCompletionListener);
    // Register a callback to be invoked when the audio file has an error to report.
    ANDROID_EXPORT virtual void setOnErrorListener(OnErrorListener);
    // Register a callback to be invoked when the audio file has samples available to client.
    ANDROID_EXPORT virtual void setOnSampleAvailableListener(OnSampleAvailableListener);

    // Prepares the audio file for decoding.
    ANDROID_EXPORT virtual void prepare();

    // Gets the properties of output audio.
    ANDROID_EXPORT std::shared_ptr<MediaFormat> getOutputFormat();

    // Writes input data to be processed to the audio file.
    ANDROID_EXPORT int64_t writeInputData(const int8_t data[], int64_t bytesCount);

    // After successfully configuring the audio file, call start.
    ANDROID_EXPORT void start();
    // Finish the decode session, note that the audio file instance remains active and ready to be start()ed again.
    ANDROID_EXPORT void stop();
    // Signals end-of-stream on input. 
    ANDROID_EXPORT void signalEndOfInputStream();

    // Resets the AudioFile to its uninitialized state.
    ANDROID_EXPORT void reset();

private:
    AudioFile();

    void stateChanged(int32_t);

    std::unique_ptr<AudioFilePrivate> m_private;
    int32_t m_state;

    OnCompletionListener m_completionListener;
    OnErrorListener m_errorListener;
    OnSampleAvailableListener m_sampleAvailableListener;
};

} // namespace appkit
} // namespace media
} // namespace android

using AudioFile = android::media::appkit::AudioFile;
