
#ifndef JPEG_DECODER_H
#define JPEG_DECODER_H

#include <error_info.h>
#include <errty.h>
#include <jerr.h>
#include <jpeg_decompressor.h>
#include <ncnm.h>
#include <tl/expected.hpp>

#include <cstdint>
#include <memory>
#include <span>

namespace img {

/**
 * @brief Alias for a non-copyable, non-movable base class.
 *
 * @tparam T Derived type using CRTP-style inheritance.
 */
template <typename T> using Default = core::NCNM<T>;

/**
 * @brief JPEG decoder error categories.
 */
enum class JpegDecoderError {

  /** Failure while reading decompressed JPEG scanlines. */
  JpegDecodeError
};

/**
 * @brief Error information type for JPEG decoder operations.
 */
using JpegDecoderErrorInfo = err::ErrorInfo<JpegDecoderError>;

/**
 * @brief Decodes JPEG scanlines into an owned pixel buffer.
 *
 * @details
 * `JpegDecoder` reads scanlines from an initialized and started
 * @c JpegDecompressor and stores the decompressed bytes in row-major order.
 * The caller provides the expected output dimensions and component count at
 * construction time.
 *
 * Copy and move operations are disabled via @c Default.
 */
class JpegDecoder : public Default<JpegDecoder> {

public:
  /**
   * @brief Constructs a decoder with storage for the expected image size.
   *
   * @param width Output image width in pixels.
   * @param height Output image height in pixels.
   * @param channels Number of color components per pixel.
   */
  explicit JpegDecoder(int width, int height, int channels) noexcept;

  /**
   * @brief Destroys the decoder and releases the pixel buffer.
   */
  ~JpegDecoder();

  /**
   * @brief Decode all remaining scanlines from a JPEG decompressor.
   *
   * @param decompressor Active decompressor positioned at scanline output.
   *
   * @return Empty expected on success, or @ref JpegDecoderErrorInfo on
   *         libjpeg scanline read failure.
   */
  tl::expected<void, JpegDecoderErrorInfo>
  decode(JpegDecompressor &decompressor) noexcept;

  /**
   * @brief Access the decoded pixel buffer.
   *
   * @return Read-only span over the complete pixel buffer.
   */
  std::span<const std::uint8_t> pixels() const noexcept;

private:
  /** @brief Number of bytes allocated for decoded pixels. */
  std::size_t size_pixels_buffer_;

  /** @brief Owned decoded pixel buffer. */
  std::unique_ptr<uint8_t[]> pixels_;

  /** @brief libjpeg error handler used during scanline decoding. */
  JpegError err_;
};

} // namespace img

#endif
