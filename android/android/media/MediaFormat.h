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

#include <java/lang.h>

#include <map>

namespace android {
namespace media {

class MediaPlayer;

class MediaFormat final {
public:  
    // (String) A key describing the mime type of the MediaFormat.
    ANDROID_EXPORT static wchar_t KEY_MIME[];
    // (Integer, optional) A key describing the maximum size in bytes of a buffer of data described by this MediaFormat.
    ANDROID_EXPORT static wchar_t KEY_MAX_INPUT_SIZE[];
    // (Integer, encoder-only) A key describing the bitrate in bits/sec.
    ANDROID_EXPORT static wchar_t KEY_BIT_RATE[];
    // (Long) A key describing the duration (in microseconds) of the content.
    ANDROID_EXPORT static wchar_t KEY_DURATION[];

    // Video formats have the following keys:

    // (Integer) A key describing the width of the content in a video format.
    ANDROID_EXPORT static wchar_t KEY_WIDTH[];
    // (Integer) A key describing the height of the content in a video format.
    ANDROID_EXPORT static wchar_t KEY_HEIGHT[];
    // (Integer) A key describing the color format of the content in a video format.
    ANDROID_EXPORT static wchar_t KEY_COLOR_FORMAT[];
    // (Integer or Float, encoder-only) A key describing the frame rate of a video format in frames/sec.
    ANDROID_EXPORT static wchar_t KEY_FRAME_RATE[];
    // (Integer) A key describing the capture rate of a video format in frames/sec.
    ANDROID_EXPORT static wchar_t KEY_CAPTURE_RATE[];
    // (Integer, encoder-only) A key describing the frequency of I frames expressed in secs between I frames.
    ANDROID_EXPORT static wchar_t KEY_I_FRAME_INTERVAL[];
    // (Integer, decoder-only) A key describing the maximum expected width of the content in a video decoder format, in case there are resolution changes in the video content.
    ANDROID_EXPORT static wchar_t KEY_MAX_WIDTH[];
    // (Integer, decoder-only) A key describing the maximum expected height of the content in a video decoder format, in case there are resolution changes in the video content.
    ANDROID_EXPORT static wchar_t KEY_MAX_HEIGHT[];

    // Audio formats have the following keys:

    // (Integer) A key describing the number of channels in an audio format.
    ANDROID_EXPORT static wchar_t KEY_CHANNEL_COUNT[];
    // (Integer) A key describing the sample rate of an audio format.
    ANDROID_EXPORT static wchar_t KEY_SAMPLE_RATE[];
    // (Integer, optional) A key mapping to a value of 1 if the content is AAC audio and audio frames are prefixed with an ADTS header.
    ANDROID_EXPORT static wchar_t KEY_IS_ADTS[];
    // (Integer, encoder-only, optional) A key describing the AAC profile to be used (AAC audio formats only).
    ANDROID_EXPORT static wchar_t KEY_AAC_PROFILE[];
    // (Integer, encoder-only, optional) A key describing the AAC SBR mode to be used (AAC audio formats only).
    ANDROID_EXPORT static wchar_t KEY_AAC_SBR_MODE[];
    // (Integer, decoder-only, optional) A key describing a gain to be applied so that the output loudness matches the Target Reference Level. 
    ANDROID_EXPORT static wchar_t KEY_AAC_DRC_TARGET_REFERENCE_LEVEL[];
    // (Integer, decoder-only, optional) A key describing the target reference level that was assumed at the encoder for calculation of attenuation gains for clipping prevention.
    ANDROID_EXPORT static wchar_t KEY_AAC_ENCODED_TARGET_LEVEL[];
    // (Integer, decoder-only, optional) A key describing the boost factor allowing to adapt the dynamics of the output to the actual listening requirements.
    ANDROID_EXPORT static wchar_t KEY_AAC_DRC_BOOST_FACTOR[];
    // (Integer, decoder-only, optional) A key describing the attenuation factor allowing to adapt the dynamics of the output to the actual listening requirements.
    ANDROID_EXPORT static wchar_t KEY_AAC_DRC_ATTENUATION_FACTOR[];
    // (Integer, decoder-only, optional) A key describing the selection of the heavy compression profile for DRC.
    ANDROID_EXPORT static wchar_t KEY_AAC_DRC_HEAVY_COMPRESSION[];
    // (Integer, decoder-only, optional) A key describing the maximum number of channels that can be output by the AAC decoder.
    ANDROID_EXPORT static wchar_t KEY_AAC_MAX_OUTPUT_CHANNEL_COUNT[];
    // (Integer, optional) A key describing the channel composition of audio content. 
    ANDROID_EXPORT static wchar_t KEY_CHANNEL_MASK[];
    // (Integer, encoder-only, optional) A key describing the FLAC compression level to be used (FLAC audio format only).
    ANDROID_EXPORT static wchar_t KEY_FLAC_COMPRESSION_LEVEL[];

    // Subtitle formats have the following keys:

    // (String) A key describing the language of the content, using either ISO 639-1 or 639-2/T codes. 
    ANDROID_EXPORT static wchar_t KEY_LANGUAGE[];

    // Mime types.

    ANDROID_EXPORT static wchar_t MIMETYPE_AUDIO_AAC[];
    ANDROID_EXPORT static wchar_t MIMETYPE_AUDIO_AC3[];
    ANDROID_EXPORT static wchar_t MIMETYPE_AUDIO_AMR_NB[];
    ANDROID_EXPORT static wchar_t MIMETYPE_AUDIO_AMR_WB[];
    ANDROID_EXPORT static wchar_t MIMETYPE_AUDIO_FLAC[];
    ANDROID_EXPORT static wchar_t MIMETYPE_AUDIO_G711_ALAW[];
    ANDROID_EXPORT static wchar_t MIMETYPE_AUDIO_G711_MLAW[];
    ANDROID_EXPORT static wchar_t MIMETYPE_AUDIO_MPEG[];
    ANDROID_EXPORT static wchar_t MIMETYPE_AUDIO_MSGSM[];
    ANDROID_EXPORT static wchar_t MIMETYPE_AUDIO_OPUS[];
    ANDROID_EXPORT static wchar_t MIMETYPE_AUDIO_QCELP[];
    ANDROID_EXPORT static wchar_t MIMETYPE_AUDIO_RAW[];
    ANDROID_EXPORT static wchar_t MIMETYPE_AUDIO_VORBIS[];
    ANDROID_EXPORT static wchar_t MIMETYPE_TEXT_CEA_608[]; // MIME type for CEA-608 closed caption data.
    ANDROID_EXPORT static wchar_t MIMETYPE_TEXT_VTT[]; // MIME type for WebVTT subtitle data.
    ANDROID_EXPORT static wchar_t MIMETYPE_VIDEO_AVC[];
    ANDROID_EXPORT static wchar_t MIMETYPE_VIDEO_H263[];
    ANDROID_EXPORT static wchar_t MIMETYPE_VIDEO_HEVC[];
    ANDROID_EXPORT static wchar_t MIMETYPE_VIDEO_MPEG2[];
    ANDROID_EXPORT static wchar_t MIMETYPE_VIDEO_MPEG4[];
    ANDROID_EXPORT static wchar_t MIMETYPE_VIDEO_RAW[];
    ANDROID_EXPORT static wchar_t MIMETYPE_VIDEO_VP8[];
    ANDROID_EXPORT static wchar_t MIMETYPE_VIDEO_VP9[];

    ANDROID_EXPORT static std::shared_ptr<MediaFormat> create();
    // Creates a minimal audio format. 
    ANDROID_EXPORT static std::shared_ptr<MediaFormat> createAudioFormat(const String& mime, int32_t sampleRate, int32_t channelCount);
    ~MediaFormat() = default;

    // Returns true if a key of the given name exists in the format.
    ANDROID_EXPORT bool containsKey(const String& name);

    // Returns the value of a float key.
    float getFloat(const String& name) { return m_floats[name]; }
    // Returns the value of an integer key.
    int32_t getInteger(const String& name) { return m_integers[name]; }
    // Returns the value of a long key.
    int64_t getLong(const String& name) { return m_longIntegers[name]; }
    // Returns the value of a string key. 
    String getString(const String& name) { return m_strings[name]; }

    // Sets the value of a float key.
    void setFloat(const String& name, float value) { m_floats[name] = value; }
    // Sets the value of an integer key.
    void setInteger(const String& name, int32_t value) { m_integers[name] = value; }
    // Sets the value of a long key.
    void setLong(const String& name, int64_t value) { m_longIntegers[name] = value; }
    // Sets the value of a string key. 
    void setString(const String& name, const String& value) { m_strings[name] = value; }

private:
    MediaFormat() = default;

    std::map<String, int32_t> m_integers;
    std::map<String, int64_t> m_longIntegers;
    std::map<String, float> m_floats;
    std::map<String, String> m_strings;
};

} // namespace media
} // namespace android

using MediaFormat = android::media::MediaFormat;
