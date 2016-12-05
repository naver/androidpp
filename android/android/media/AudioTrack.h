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

#include <android/media/AudioFormat.h>
#include <android/os/Handler.h>

namespace android {
namespace media {

class AudioTrackPrivate;

class AudioTrack {
    friend class AudioTrackPrivate;
public:
    // Denotes a successful operation.
    static const int32_t SUCCESS = 0;
    // Denotes a generic operation failure.
    static const int32_t ERROR = -1;
    // Denotes a failure due to the use of an invalid value.
    static const int32_t ERROR_BAD_VALUE = -2;
    // Denotes a failure due to the improper use of a method.
    static const int32_t ERROR_INVALID_OPERATION = -3;
    // Creation mode where audio data is transferred from Java to the native layer only once before the audio starts playing.
    static const int32_t MODE_STATIC = 0;
    // Creation mode where audio data is streamed from Java to the native layer as the audio is playing.
    static const int32_t MODE_STREAM = 1;
    // Indicates AudioTrack state is stopped.
    static const int32_t PLAYSTATE_STOPPED = 1;
    // Indicates AudioTrack state is paused.
    static const int32_t PLAYSTATE_PAUSED = 2;
    // Indicates AudioTrack state is playing.
    static const int32_t PLAYSTATE_PLAYING = 3;
    // State of an AudioTrack that was not successfully initialized upon creation.
    static const int32_t STATE_UNINITIALIZED = 0;
    // State of an AudioTrack that is ready to be used.
    static const int32_t STATE_INITIALIZED = 1;
    // State of a successfully initialized AudioTrack that uses static data, but that hasn't received that data yet.
    static const int32_t STATE_NO_STATIC_DATA = 2;
    // The write mode indicating the write operation will block until all data has been written, to be used in write(ByteBuffer, int32_t, int32_t)
    static const int32_t WRITE_BLOCKING = 0;
    // The write mode indicating the write operation will return immediately after queuing as much audio data for playback as possible without blocking, to be used in write(ByteBuffer, int32_t, int32_t). 
    static const int32_t WRITE_NON_BLOCKING = 1;

    ANDROID_EXPORT static std::shared_ptr<AudioTrack> create(int32_t streamType, int32_t sampleRateInHz, int32_t channelConfig, int32_t audioFormat, int32_t bufferSizeInBytes, int32_t mode);
    ANDROID_EXPORT virtual ~AudioTrack();

    class OnPlaybackPositionUpdateListener {
    public:
        std::function<void (AudioTrack&)> onMarkerReached;
        std::function<void (AudioTrack&)> onPeriodicNotification;
    };

    // Returns the maximum gain value, which is greater than or equal to 1.0.
    ANDROID_EXPORT static float getMaxVolume();
    // Returns the minimum gain value, which is the constant 0.0.
    ANDROID_EXPORT static float getMinVolume();
    // Returns the output sample rate in Hz for the specified stream type.
    ANDROID_EXPORT static int32_t getNativeOutputSampleRate(int32_t streamType);
    // Returns the minimum buffer size required for the successful creation of an AudioTrack object to be created in the MODE_STREAM mode.
    ANDROID_EXPORT static int32_t getMinBufferSize(int32_t sampleRateInHz, int32_t channelConfig, int32_t audioFormat);

    // Returns the configured audio data format.
    ANDROID_EXPORT virtual int32_t getAudioFormat();
    // Returns the configured channel configuration.
    ANDROID_EXPORT virtual int32_t getChannelConfiguration();
    // Returns the configured number of channels.
    ANDROID_EXPORT virtual int32_t getChannelCount();
    // Returns the playback state of the AudioTrack instance.
    ANDROID_EXPORT virtual int32_t getPlayState();
    // Returns the configured audio data sample rate in Hz
    ANDROID_EXPORT virtual int32_t getSampleRate();
    // Returns the state of the AudioTrack instance.
    ANDROID_EXPORT virtual int32_t getState();
    // Returns the type of audio stream this AudioTrack is configured for.
    ANDROID_EXPORT virtual int32_t getStreamType();

    // Returns the playback head position expressed in frames.
    ANDROID_EXPORT virtual int32_t getPlaybackHeadPosition();
    // Returns the current playback rate in Hz.
    ANDROID_EXPORT virtual int32_t getPlaybackRate();
    // Sets the playback head position.
    ANDROID_EXPORT virtual int32_t setPlaybackHeadPosition(int32_t positionInFrames);
    // Sets the playback sample rate for this track.
    ANDROID_EXPORT virtual int32_t setPlaybackRate(int32_t sampleRateInHz);
    // Sets the specified output gain value on all channels of this track.
    ANDROID_EXPORT virtual int32_t setVolume(float gain);

    // Returns marker position expressed in frames.
    ANDROID_EXPORT virtual int32_t getNotificationMarkerPosition();
    // Returns the notification update period expressed in frames.
    ANDROID_EXPORT virtual int32_t getPositionNotificationPeriod();
    // Sets the position of the notification marker.
    ANDROID_EXPORT virtual int32_t setNotificationMarkerPosition(int32_t markerInFrames);
    // Sets the period for the periodic notification event.
    ANDROID_EXPORT virtual int32_t setPositionNotificationPeriod(int32_t periodInFrames);
    // Sets the listener the AudioTrack notifies when a previously set marker is reached or for each periodic playback head position update.
    ANDROID_EXPORT virtual void setPlaybackPositionUpdateListener(OnPlaybackPositionUpdateListener listener);
    // Sets the listener the AudioTrack notifies when a previously set marker is reached or for each periodic playback head position update.
    ANDROID_EXPORT virtual void setPlaybackPositionUpdateListener(OnPlaybackPositionUpdateListener listener, std::shared_ptr<os::Handler>& handler);

    // Starts playing an AudioTrack.
    ANDROID_EXPORT virtual void play();
    // Pauses the playback of the audio data.
    ANDROID_EXPORT virtual void pause();
    // Stops playing the audio data.
    ANDROID_EXPORT virtual void stop();
    // Releases the native AudioTrack resources.
    ANDROID_EXPORT virtual void release();
    // Flushes the audio data currently queued for playback.
    ANDROID_EXPORT virtual void flush();

    // Writes the audio data to the audio sink for playback (streaming mode);, or copies audio data for later playback (static buffer mode);.
    ANDROID_EXPORT virtual int32_t write(float audioData[], int32_t offsetInFloats, int32_t sizeInFloats, int32_t writeMode);

private:
    AudioTrack(int32_t streamType, int32_t sampleRateInHz, int32_t channelConfig, int32_t audioFormat, int32_t bufferSizeInBytes, int32_t mode);

    std::unique_ptr<AudioTrackPrivate> m_private;
    int32_t m_playState;
    int32_t m_state;

    int32_t m_streamType;
    int32_t m_sampleRateInHz;
    int32_t m_channelConfig;
    int32_t m_audioFormat;
    int32_t m_bufferSizeInBytes;
    int32_t m_mode;

    OnPlaybackPositionUpdateListener m_playbackPositionUpdateListener;
    std::shared_ptr<os::Handler> m_playbackPositionUpdateHandler;
};

} // namespace media
} // namespace android

using AudioTrack = android::media::AudioTrack;
