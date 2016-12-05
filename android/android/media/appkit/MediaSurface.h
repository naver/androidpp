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

#include <android/media/MediaPlayer.h>
#include <android/opengl/appkit/GLTexture.h>
#include <android/opengl/appkit/GLTransportSurface.h>
#include <platforms/Functional.h>

namespace android {
namespace media {
namespace appkit {

class MediaSurface : public GLTransportSurface::Backbuffer {
public:  
    ANDROID_EXPORT static std::shared_ptr<MediaSurface> create();
    ANDROID_EXPORT virtual ~MediaSurface();

    typedef std::safe_function<void (MediaSurface&)> OnFrameAvailableListener;

    ANDROID_EXPORT virtual void setSourceMedia(std::shared_ptr<MediaPlayer>&);
    ANDROID_EXPORT virtual void setOutputSurface(std::shared_ptr<GLTransportSurface>);

    // Register a callback to be invoked when a frame is available.
    ANDROID_EXPORT virtual void setOnFrameAvailableListener(OnFrameAvailableListener);

    // Resets the MediaSurface to its uninitialized state.
    ANDROID_EXPORT virtual void reset();

    // GLTransportSurface::Backbuffer

    // Copy most recent frame from the media player onto the output surface here. Must be called on the painting thread.
    ANDROID_EXPORT virtual void swapBuffers(GLTransportSurface&);

private:
    void frameAvailable();

    void attachCurrentSourceMedia();
    void detachCurrentSourceMedia();
    std::shared_ptr<GLTexture> frameTexture();

protected:
    MediaSurface();

    std::shared_ptr<MediaPlayer> m_mediaPlayer;
    std::shared_ptr<GLTransportSurface> m_outputSurface;
    OnFrameAvailableListener m_frameAvailableListener;
};

} // namespace appkit
} // namespace media
} // namespace android

using MediaSurface = android::media::appkit::MediaSurface;
