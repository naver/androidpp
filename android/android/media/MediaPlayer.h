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

#include <android/content/Context.h>
#include <platforms/Functional.h>

#include <map>

namespace android {
namespace media {

class MediaPlayerPrivate;

class MediaPlayer {
    friend class MediaPlayerPrivate;
public:  
    // File or network related operation errors.
    static const int32_t MEDIA_ERROR_IO = 0xfffffc14;
    // Bitstream is not conforming to the related coding standard or file spec.
    static const int32_t MEDIA_ERROR_MALFORMED = 0xfffffc11;
    // The video is streamed and its container is not valid for progressive playback i.e the video's index (e.g moov atom) is not at the start of the file.
    static const int32_t MEDIA_ERROR_NOT_VALID_FOR_PROGRESSIVE_PLAYBACK = 0x000000c8;
    // Media server died.
    static const int32_t MEDIA_ERROR_SERVER_DIED = 0x00000064;
    // Some operation takes too int64_t to complete, usually more than 3-5 seconds.
    static const int32_t MEDIA_ERROR_TIMED_OUT = 0xffffff92;
    // Unspecified media player error.
    static const int32_t MEDIA_ERROR_UNKNOWN = 0x00000001;
    // Bitstream is conforming to the related coding standard or file spec, but the media framework does not support the feature.
    static const int32_t MEDIA_ERROR_UNSUPPORTED = 0xfffffc0e;
    // Bad interleaving means that a media has been improperly interleaved or not interleaved at all, e.g has all the video samples first then all the audio ones.
    static const int32_t MEDIA_INFO_BAD_INTERLEAVING = 0x00000320;
    // MediaPlayer is resuming playback after filling buffers.
    static const int32_t MEDIA_INFO_BUFFERING_END = 0x000002be;
    // MediaPlayer is temporarily pausing playback internally in order to buffer more data.
    static const int32_t MEDIA_INFO_BUFFERING_START = 0x000002bd;
    // A new set of metadata is available.
    static const int32_t MEDIA_INFO_METADATA_UPDATE = 0x00000322;
    // The media cannot be seeked (e.g live stream)
    static const int32_t MEDIA_INFO_NOT_SEEKABLE = 0x00000321;
    // Reading the subtitle track takes too int64_t.
    static const int32_t MEDIA_INFO_SUBTITLE_TIMED_OUT = 0x00000386;
    // Unspecified media player info.
    static const int32_t MEDIA_INFO_UNKNOWN = 0x00000001;
    // Subtitle track was not supported by the media framework.
    static const int32_t MEDIA_INFO_UNSUPPORTED_SUBTITLE = 0x00000385;
    // The player just pushed the very first video frame for rendering.
    static const int32_t MEDIA_INFO_VIDEO_RENDERING_START = 0x00000003;
    // The video is too complex for the decoder: it can't decode frames fast enough.
    static const int32_t MEDIA_INFO_VIDEO_TRACK_LAGGING = 0x000002bc;

    // Specifies a video scaling mode.
    static const int32_t VIDEO_SCALING_MODE_SCALE_TO_FIT = 0x00000001;
    // Specifies a video scaling mode.
    static const int32_t VIDEO_SCALING_MODE_SCALE_TO_FIT_WITH_CROPPING = 0x00000002;

    // Default constructor.
    ANDROID_EXPORT static std::shared_ptr<MediaPlayer> create();
    // Convenience method to create a MediaPlayer for a given uri.
    ANDROID_EXPORT static std::shared_ptr<MediaPlayer> create(String& uri);
    ANDROID_EXPORT virtual ~MediaPlayer();

    typedef std::function<void (int32_t)> OnBufferingUpdateListener;
    typedef std::function<void ()> OnCompletionListener;
    typedef std::function<void (int32_t)> OnErrorListener;
    typedef std::function<void ()> OnInfoListener;
    typedef std::function<void ()> OnPreparedListener;
    typedef std::function<void ()> OnSeekCompleteListener;
    typedef std::function<void ()> OnTimedTextListener;
    typedef std::function<void (int32_t, int32_t)> OnVideoSizeChangedListener;

    struct TrackInfo {
        static const int32_t MEDIA_TRACK_TYPE_UNKNOWN = 0;
        static const int32_t MEDIA_TRACK_TYPE_VIDEO = 1;
        static const int32_t MEDIA_TRACK_TYPE_AUDIO = 2;
        static const int32_t MEDIA_TRACK_TYPE_TIMEDTEXT = 3;

        int32_t trackType;
        String language;

        TrackInfo()
            : trackType(MEDIA_TRACK_TYPE_UNKNOWN), language(L"")
        { }
    };

    // Adds an external timed text source file.
    ANDROID_EXPORT virtual void addTimedTextSource(String& path, String& mimeType);
    // Deselect a track.
    ANDROID_EXPORT virtual void deselectTrack(int32_t index);

    // Gets the current playback position.
    ANDROID_EXPORT virtual int32_t getCurrentPosition();
    // Gets the duration of the file.
    ANDROID_EXPORT virtual int32_t getDuration();
    // Returns the index of the audio, video, or subtitle track currently selected for playback, The return value is an index into the array returned by getTrackInfo(), and can be used in calls to selectTrack(int32_t) or deselectTrack(int32_t).
    ANDROID_EXPORT virtual int32_t getSelectedTrack(int32_t trackType);
    // Returns an array of track information.
    ANDROID_EXPORT virtual std::vector<TrackInfo> getTrackInfo();
    // Returns the height of the video.
    ANDROID_EXPORT virtual int32_t getVideoHeight();
    // Returns the width of the video.
    ANDROID_EXPORT virtual int32_t getVideoWidth();

    // Checks whether the MediaPlayer is looping or non-looping.
    ANDROID_EXPORT virtual bool isLooping();
    // Checks whether the MediaPlayer is playing.
    ANDROID_EXPORT virtual bool isPlaying();

    // Pauses playback.
    ANDROID_EXPORT virtual void pause();
    // Prepares the player for playback, synchronously.
    ANDROID_EXPORT virtual void prepare();
    // Prepares the player for playback, asynchronously.
    ANDROID_EXPORT virtual void prepareAsync();
    // Releases resources associated with this MediaPlayer object.
    ANDROID_EXPORT virtual void release();
    // Resets the MediaPlayer to its uninitialized state.
    ANDROID_EXPORT virtual void reset();
    // Seeks to specified time position.
    ANDROID_EXPORT virtual void seekTo(int32_t msec);
    // Selects a track.
    ANDROID_EXPORT virtual void selectTrack(int32_t index);

    // Sets the data source (file-path or http/rtsp URL) to use.
    ANDROID_EXPORT virtual void setDataSource(String& path);
    // Sets the data source as a content Uri.
    ANDROID_EXPORT virtual void setDataSource(Context& context, String& uri, const std::map<String, String>& headers);

    // Sets the player to be looping or non-looping.
    ANDROID_EXPORT virtual void setLooping(bool looping);
    // Register a callback to be invoked when the status of a network stream's buffer has changed.
    ANDROID_EXPORT virtual void setOnBufferingUpdateListener(OnBufferingUpdateListener listener);
    // Register a callback to be invoked when the end of a media source has been reached during playback.
    ANDROID_EXPORT virtual void setOnCompletionListener(OnCompletionListener listener);
    // Register a callback to be invoked when an error has happened during an asynchronous operation.
    ANDROID_EXPORT virtual void setOnErrorListener(OnErrorListener listener);
    // Register a callback to be invoked when an info/warning is available.
    ANDROID_EXPORT virtual void setOnInfoListener(OnInfoListener listener);
    // Register a callback to be invoked when the media source is ready for playback.
    ANDROID_EXPORT virtual void setOnPreparedListener(OnPreparedListener listener);
    // Register a callback to be invoked when a seek operation has been completed.
    ANDROID_EXPORT virtual void setOnSeekCompleteListener(OnSeekCompleteListener listener);
    // Register a callback to be invoked when a timed text is available for display.
    ANDROID_EXPORT virtual void setOnTimedTextListener(OnTimedTextListener listener);
    // Register a callback to be invoked when the video size is known or updated. 
    ANDROID_EXPORT virtual void setOnVideoSizeChangedListener(OnVideoSizeChangedListener listener);
    // Control whether we should use the attached SurfaceHolder to keep the screen on while video playback is occurring.
    ANDROID_EXPORT virtual void setScreenOnWhilePlaying(bool screenOn);
    // Sets video scaling mode.
    ANDROID_EXPORT virtual void setVideoScalingMode(int32_t mode);
    // Sets the volume on this player.
    ANDROID_EXPORT virtual void setVolume(float leftVolume, float rightVolume);
    // Set the low-level power management behavior for this MediaPlayer.
    ANDROID_EXPORT virtual void setWakeMode(int32_t mode);

    // Starts or resumes playback.
    ANDROID_EXPORT virtual void start();
    // Stops playback after playback has been stopped or paused. 
    ANDROID_EXPORT virtual void stop();

private:
    MediaPlayer();

    void stateChanged(int32_t);

    std::unique_ptr<MediaPlayerPrivate> m_private;
    int32_t m_state;

    OnBufferingUpdateListener m_bufferingUpdateListener;
    OnCompletionListener m_completionListener;
    OnErrorListener m_errorListener;
    OnInfoListener m_infoListener;
    OnPreparedListener m_preparedListener;
    OnSeekCompleteListener m_seekCompleteListener;
    OnTimedTextListener m_timedTextListener;
    OnVideoSizeChangedListener m_videoSizeChangedListener;
};

} // namespace media
} // namespace android

using MediaPlayer = android::media::MediaPlayer;
