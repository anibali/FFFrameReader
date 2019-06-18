/**
 * Copyright 2019 Matthew Oliver
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include <any>
#include <cstdint>
#include <memory>

struct AVFormatContext;
struct AVCodecContext;
struct AVFrame;

namespace Ffr {
enum class DecodeType
{
    Software,
    Cuda,
};

enum class EncodeType
{
    h264,
    h265,
};

struct Resolution
{
    uint32_t m_width;
    uint32_t m_height;
};

struct Crop
{
    uint32_t m_top;    /**< The offset in pixels from top of frame */
    uint32_t m_bottom; /**< The offset in pixels from bottom of frame */
    uint32_t m_left;   /**< The offset in pixels from left of frame */
    uint32_t m_right;  /**< The offset in pixels from right of frame */
};

enum class PixelFormat : int32_t
{
    Auto = -1,   /**< Keep pixel format same as input */
    YUV420P = 0, /**< planar YUV 4:2:0, 12bpp, (1 Cr & Cb sample per 2x2 Y samples) */
    YUV422P = 4, /**< planar YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples) */
    YUV444P = 5, /**< planar YUV 4:4:4, 24bpp, (1 Cr & Cb sample per 1x1 Y samples) */
    NV12 = 23,   /**< planar YUV 4:2:0, 12bpp, 1 plane for Y and 1 plane for the UV components, which are interleaved
                    (first byte U and the following byte V) */

    RGB8P = 73, /**< planar RGB 4:4:4 24bpp */

    RGB32FP = 178, /**< IEEE-754 single precision planar RGB 4:4:4, 96bpp */

    RGB8 = 2, /**< packed RGB 8:8:8, 24bpp, RGBRGB... */
};

class DecoderOptions
{
public:
    DecoderOptions() = default;

    explicit DecoderOptions(DecodeType type) noexcept;

    ~DecoderOptions() = default;

    DecoderOptions(const DecoderOptions& other) = default;

    DecoderOptions(DecoderOptions&& other) = default;

    DecoderOptions& operator=(const DecoderOptions& other) = default;

    DecoderOptions& operator=(DecoderOptions&& other) = default;

    bool operator==(const DecoderOptions& other) const noexcept;

    bool operator!=(const DecoderOptions& other) const noexcept;

    bool operator<(const DecoderOptions& other) const noexcept;

    DecodeType m_type = DecodeType::Software; /**< The type of decoding to use. */
    Crop m_crop = {0, 0, 0, 0};               /**< The output cropping or (0) if no crop should be performed. */
    Resolution m_scale = {0, 0}; /**< The output resolution or (0, 0) if no scaling should be performed. Scaling is
                                    performed after cropping. */
    PixelFormat m_format = PixelFormat::Auto; /**< The required output pixel format (auto to keep format the same). */
    uint32_t m_bufferLength = 10;             /**< Number of frames in the the decode buffer.
                                              This should be optimised based on reading/seeking pattern so as to minimise frame
                                              storage requirements but also maximise decode throughput. */
    std::any m_context;                       /**< Pointer to an existing context to be used for hardware
                                               decoding. This must match the hardware type specified in @m_type. */
    uint32_t m_device = 0;                    /**< The device index for the desired hardware device. */
    bool m_outputHost = true; /**< True to output each frame to host CPU memory (only affects hardware decoding). */
};

class EncoderOptions
{
public:
    enum class Preset
    {
        Ultrafast,
        Superfast,
        Veryfast,
        Faster,
        Fast,
        Medium,
        Slow,
        Slower,
        Veryslow,
        Placebo,
    };

    EncoderOptions() = default;

    explicit EncoderOptions(DecodeType type) noexcept;

    ~EncoderOptions() = default;

    EncoderOptions(const EncoderOptions& other) = default;

    EncoderOptions(EncoderOptions&& other) = default;

    EncoderOptions& operator=(const EncoderOptions& other) = default;

    EncoderOptions& operator=(EncoderOptions&& other) = default;

    bool operator==(const EncoderOptions& other) const noexcept;

    bool operator!=(const EncoderOptions& other) const noexcept;

    bool operator<(const EncoderOptions& other) const noexcept;

    EncodeType m_type = EncodeType::h264; /**< The type of encoder to use. */
    uint8_t m_quality = 125;              /**< The quality of the output video. 0 is worst, 255 is best. */
    Preset m_preset = Preset::Medium; /**< The preset compression level to use. Higher values result in smaller files
                             but increased encoding time. */
};

class FormatContextPtr
{
    friend class Stream;
    friend class Filter;
    friend class Encoder;
    friend class StreamUtils;
    friend class Frame;

    FormatContextPtr() = default;

    explicit FormatContextPtr(AVFormatContext* formatContext) noexcept;

    [[nodiscard]] AVFormatContext* get() const noexcept;

    AVFormatContext* operator->() const noexcept;

    std::shared_ptr<AVFormatContext> m_formatContext = nullptr;
};

class CodecContextPtr
{
    friend class Stream;
    friend class Filter;
    friend class Encoder;
    friend class StreamUtils;
    friend class Frame;

    CodecContextPtr() = default;

    explicit CodecContextPtr(AVCodecContext* codecContext) noexcept;

    [[nodiscard]] AVCodecContext* get() const noexcept;

    AVCodecContext* operator->() const noexcept;

    std::shared_ptr<AVCodecContext> m_codecContext = nullptr;
};

class FramePtr
{
    friend class Frame;
    friend class Filter;
    friend class Stream;
    friend class Encoder;
    friend class StreamUtils;
    friend class FFR;

public:
    ~FramePtr() noexcept;

    FramePtr(const FramePtr& other) noexcept = delete;

private:
    FramePtr() noexcept = default;

    explicit FramePtr(AVFrame* frame) noexcept;

    FramePtr(FramePtr&& other) noexcept;

    FramePtr& operator=(FramePtr& other) noexcept;

    FramePtr& operator=(FramePtr&& other) noexcept;

    AVFrame*& operator*() noexcept;

    const AVFrame* operator*() const noexcept;

    AVFrame*& operator->() noexcept;

    const AVFrame* operator->() const noexcept;

    AVFrame* m_frame = nullptr;
};

class OutputFormatContextPtr
{
    friend class Encoder;

    OutputFormatContextPtr() = default;

    explicit OutputFormatContextPtr(AVFormatContext* formatContext) noexcept;

    [[nodiscard]] AVFormatContext* get() const noexcept;

    AVFormatContext* operator->() const noexcept;

    std::shared_ptr<AVFormatContext> m_formatContext = nullptr;
};
} // namespace Ffr
