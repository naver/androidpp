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

#include "MediaPlayerPrivate.h"

namespace android {
namespace media {

class MediaPlayerPrivateMock final : public MediaPlayerPrivate {
public:  
    static const int32_t mediaDuration = 45000;
    static const int32_t mediaPrepareLatency = 1000;
    static const int32_t mediaInitialBuffering = 15;
    static const int32_t mediaBufferingIncrement = 10;
    static const int32_t mediaBufferingLatency = 1000;
    static const int32_t mediaSeekingLatency = 300;
    static const int32_t mediaVideoWidth = 600;
    static const int32_t mediaVideoHeight = 400;
    static const int32_t mediaFrameAvailableLatency = 1000 / 30;

    MediaPlayerPrivateMock(MediaPlayer&);
    ~MediaPlayerPrivateMock();

    void setDataSource(const String&) override;
    void setVolume(float) override;

    int32_t getDuration() override;
    int32_t getCurrentPosition() override;
    void seekTo(int32_t msec) override;

    int32_t getVideoHeight() override;
    int32_t getVideoWidth() override;

    void stateChanged(State oldState, State newState) override;

    void copyFrame(GLTexture&) override;

private:
    void post(std::function<void ()> callback, int32_t delayMillis);

    void reset();

    void onBufferingUpdate(int32_t percent);
    void onCompletion();
    void onFrameAvailable();
    void onPrepared();
    void onSeekComplete(int32_t msec);
    void onVideoSizeChanged();

    void onUpdatePlayback();

    State m_state;
    int32_t m_pendingCallbacks;
    bool m_suppressCallbacks;

    String m_path;
    int32_t m_duration;
    int32_t m_buffering;
    int32_t m_currentTime;
    int32_t m_currentPosition;
};

} // namespace media
} // namespace android
