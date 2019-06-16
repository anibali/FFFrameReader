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
#include "FFFRTypes.h"

#include <cstdint>

namespace Ffr {
class Frame
{
    friend class Stream;
    friend class Filter;
    friend class Encoder;
    friend class StreamUtils;
    friend class FFR;

public:
    Frame() = default;

    /**
     * Constructor.
     * @param [in,out] frame     The frame pointer to frame data. This is reset to nullptr on function exit.
     * @param          timeStamp The time stamp for the current frame.
     * @param          frameNum  The zero-indexed frame number in the stream.
     */
    Frame(FramePtr& frame, int64_t timeStamp, int64_t frameNum) noexcept;

    ~Frame() noexcept = default;

    Frame(const Frame& other) noexcept = delete;

    Frame(Frame&& other) noexcept = default;

    Frame& operator=(Frame& other) noexcept = default;

    Frame& operator=(Frame&& other) noexcept = default;

    /**
     * Gets global time stamp for frame.
     * @returns The time stamp.
     */
    [[nodiscard]] int64_t getTimeStamp() const noexcept;

    /**
     * Gets picture sequence frame number.
     * @returns The frame number.
     */
    [[nodiscard]] int64_t getFrameNumber() const noexcept;

    /**
     * Gets frame data pointer for a specified image plane. The memory address of these frames will be in a memory space
     * dependent on what settings where used when the parent stream was created. (e.g. cuda pointer if using nvdec
     * etc.).
     * @param plane The image plane to get. Should be less than @getNumberFrames.
     * @returns The internal frame data and line size, nullptr if specified frame does not exist.
     */
    [[nodiscard]] std::pair<uint8_t* const, int32_t> getFrameData(uint32_t plane) const noexcept;

    /**
     * Gets the frame width.
     * @returns The width.
     */
    [[nodiscard]] uint32_t getWidth() const noexcept;

    /**
     * Gets the frame height.
     * @returns The height.
     */
    [[nodiscard]] uint32_t getHeight() const noexcept;

    /**
     * Gets the display aspect ratio of the video stream.
     * @note This may differ from width/height if stream uses anamorphic pixels.
     * @returns The aspect ratio.
     */
    [[nodiscard]] double getAspectRatio() const noexcept;

    /**
     * Gets the pixel format of the frame data.
     * @note This is used to determine the format of the data returned by @getFrameData.
     * @returns The pixel format.
     */
    [[nodiscard]] PixelFormat getPixelFormat() const noexcept;

    /**
     * Gets number of planes for an image of the specified pixel format
     * @returns The number of planes (YUV420P has 3, RGB8 has 1 etc.) or negative value if invalid format.
     */
    [[nodiscard]] int32_t getNumberFrames() const noexcept;

    /**
     * Gets the type of memory used to store the image.
     * @returns The data type.
     */
    [[nodiscard]] DecodeType getDataType() const noexcept;

private:
    FramePtr m_frame;
    int64_t m_timeStamp = 0;
    int64_t m_frameNum = 0;
};
} // namespace Ffr