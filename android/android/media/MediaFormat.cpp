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

#include "MediaFormat.h"

namespace android {
namespace media {

wchar_t MediaFormat::KEY_MIME[] = L"mime";
wchar_t MediaFormat::KEY_MAX_INPUT_SIZE[] = L"max-input-size";
wchar_t MediaFormat::KEY_BIT_RATE[] = L"bitrate";
wchar_t MediaFormat::KEY_DURATION[] = L"durationUs";

wchar_t MediaFormat::KEY_WIDTH[] = L"width";
wchar_t MediaFormat::KEY_HEIGHT[] = L"height";
wchar_t MediaFormat::KEY_COLOR_FORMAT[] = L"color-format";
wchar_t MediaFormat::KEY_FRAME_RATE[] = L"frame-rate";
wchar_t MediaFormat::KEY_CAPTURE_RATE[] = L"capture-rate";
wchar_t MediaFormat::KEY_I_FRAME_INTERVAL[] = L"i-frame-interval";
wchar_t MediaFormat::KEY_MAX_WIDTH[] = L"max-width";
wchar_t MediaFormat::KEY_MAX_HEIGHT[] = L"max-height";

wchar_t MediaFormat::KEY_CHANNEL_COUNT[] = L"channel-count";
wchar_t MediaFormat::KEY_SAMPLE_RATE[] = L"sample-rate";
wchar_t MediaFormat::KEY_IS_ADTS[] = L"is-adts";
wchar_t MediaFormat::KEY_AAC_PROFILE[] = L"aac-profile";
wchar_t MediaFormat::KEY_AAC_SBR_MODE[] = L"aac-sbr-mode";
wchar_t MediaFormat::KEY_AAC_DRC_TARGET_REFERENCE_LEVEL[] = L"aac-target-ref-level";
wchar_t MediaFormat::KEY_AAC_ENCODED_TARGET_LEVEL[] = L"aac-encoded-target-level";
wchar_t MediaFormat::KEY_AAC_DRC_BOOST_FACTOR[] = L"aac-drc-boost-level";
wchar_t MediaFormat::KEY_AAC_DRC_ATTENUATION_FACTOR[] = L"aac-drc-cut-level";
wchar_t MediaFormat::KEY_AAC_DRC_HEAVY_COMPRESSION[] = L"aac-drc-heavy-compression";
wchar_t MediaFormat::KEY_AAC_MAX_OUTPUT_CHANNEL_COUNT[] = L"aac-max-output-channel_count";
wchar_t MediaFormat::KEY_CHANNEL_MASK[] = L"channel-mask";
wchar_t MediaFormat::KEY_FLAC_COMPRESSION_LEVEL[] = L"flac-compression-level";

wchar_t MediaFormat::KEY_LANGUAGE[] = L"language";

wchar_t MediaFormat::MIMETYPE_AUDIO_AAC[] = L"audio/aac";
wchar_t MediaFormat::MIMETYPE_AUDIO_AC3[] = L"audio/ac3";
wchar_t MediaFormat::MIMETYPE_AUDIO_AMR_NB[] = L"audio/3gpp";
wchar_t MediaFormat::MIMETYPE_AUDIO_AMR_WB[] = L"audio/amr-wb";
wchar_t MediaFormat::MIMETYPE_AUDIO_FLAC[] = L"audio/flac";
wchar_t MediaFormat::MIMETYPE_AUDIO_G711_ALAW[] = L"audio/g711-alaw";
wchar_t MediaFormat::MIMETYPE_AUDIO_G711_MLAW[] = L"audio/g711-mlaw";
wchar_t MediaFormat::MIMETYPE_AUDIO_MPEG[] = L"audio/mpeg";
wchar_t MediaFormat::MIMETYPE_AUDIO_MSGSM[] = L"audio/gsm";
wchar_t MediaFormat::MIMETYPE_AUDIO_OPUS[] = L"audio/opus";
wchar_t MediaFormat::MIMETYPE_AUDIO_QCELP[] = L"audio/qcelp";
wchar_t MediaFormat::MIMETYPE_AUDIO_RAW[] = L"audio/raw";
wchar_t MediaFormat::MIMETYPE_AUDIO_VORBIS[] = L"audio/vorbis";
wchar_t MediaFormat::MIMETYPE_TEXT_CEA_608[] = L"text/cea-608"; // MIME type for CEA-608 closed caption data.
wchar_t MediaFormat::MIMETYPE_TEXT_VTT[] = L"text/vtt"; // MIME type for WebVTT subtitle data.
wchar_t MediaFormat::MIMETYPE_VIDEO_AVC[] = L"video/avc";
wchar_t MediaFormat::MIMETYPE_VIDEO_H263[] = L"video/3gpp";
wchar_t MediaFormat::MIMETYPE_VIDEO_HEVC[] = L"video/hevc";
wchar_t MediaFormat::MIMETYPE_VIDEO_MPEG2[] = L"video/mpeg2";
wchar_t MediaFormat::MIMETYPE_VIDEO_MPEG4[] = L"video/mp4v-es";
wchar_t MediaFormat::MIMETYPE_VIDEO_RAW[] = L"video/raw";
wchar_t MediaFormat::MIMETYPE_VIDEO_VP8[] = L"video/x-vnd.on2.vp8";
wchar_t MediaFormat::MIMETYPE_VIDEO_VP9[] = L"video/x-vnd.on2.vp9";

std::shared_ptr<MediaFormat> MediaFormat::create()
{
    return std::shared_ptr<MediaFormat>(new MediaFormat);
}

std::shared_ptr<MediaFormat> MediaFormat::createAudioFormat(const String& mime, int32_t sampleRate, int32_t channelCount)
{
    std::shared_ptr<MediaFormat> format = create();
    format->setString(KEY_MIME, mime);
    format->setInteger(KEY_SAMPLE_RATE, sampleRate);
    format->setInteger(KEY_CHANNEL_COUNT, channelCount);
    return std::move(format);
}

// Returns true if a key of the given name exists in the format.
bool MediaFormat::containsKey(const String& name)
{
    return (m_integers.count(name) > 0 || m_longIntegers.count(name) > 0 || m_floats.count(name) > 0 || m_strings.count(name) > 0);
}

} // namespace media
} // namespace android
