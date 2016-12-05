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

#include <android/opengl/appkit/GLTexture.h>
#include <platforms/Functional.h>

namespace android {
namespace media {

class MediaPlayerPrivate : public Object {
    friend class MediaPlayer;
public:  
    static std::unique_ptr<MediaPlayerPrivate> create(MediaPlayer&);
    virtual ~MediaPlayerPrivate() = default;

    ANDROID_EXPORT static void setFactory(MediaPlayerPrivate* (*)(MediaPlayer&));

    ANDROID_EXPORT static MediaPlayerPrivate& getPrivate(MediaPlayer&);

    enum State {
        Unknown,
        Idle,
        Initialized,
        Preparing,
        Prepared,
        Started,
        Paused,
        Stopped,
        PlaybackCompleted,
        End,
        Error,
    };

    virtual void setDataSource(const String& path) = 0;
    virtual void setVolume(float) = 0;

    virtual int32_t getDuration() = 0;
    virtual int32_t getCurrentPosition() = 0;
    virtual void seekTo(int32_t msec) = 0;

    virtual int32_t getVideoHeight() = 0;
    virtual int32_t getVideoWidth() = 0;

    // Called by MediaPlayer whenever its state is being changed.
    virtual void stateChanged(State oldState, State newState) = 0;

    virtual void setOnFrameAvailableListener(std::function<void ()> l) { m_frameAvailableListener = std::move(l); }
    virtual void copyFrame(GLTexture&) = 0;

protected:
    MediaPlayerPrivate(MediaPlayer& mp)
        : m_player(mp)
    {
    }

    ANDROID_EXPORT void callOnBufferingUpdateListener(int32_t percent);
    ANDROID_EXPORT void callOnCompletionListener();
    ANDROID_EXPORT void callOnErrorListener(int32_t error);
    ANDROID_EXPORT void callOnInfoListener();
    ANDROID_EXPORT void callOnPreparedListener();
    ANDROID_EXPORT void callOnSeekCompleteListener();
    ANDROID_EXPORT void callOnTimedTextListener();
    ANDROID_EXPORT void callOnVideoSizeChangedListener(int32_t width, int32_t height);

    media::MediaPlayer& m_player;
    std::safe_function<void ()> m_frameAvailableListener;
};

} // namespace media
} // namespace android
