
#ifndef JPEG_COMPRESSOR_H
#define JPEG_COMPRESSOR_H

#include <error_info.h>
#include <errty.h>
#include <inputimg.h>
#include <jerr.h>
#include <nc.h>
#include <tl/expected.hpp>

namespace img {

/**
 * @brief Alias for a non-copyable base class.
 *
 * @tparam T Derived type.
 */
template <typename T> using Movable = core::NC<T>;

enum class JpegCompressorError {
  InitCompressionError,
  CompressionError,
};

using JpegCompressorErrorInfo = err::ErrorInfo<JpegCompressorError>;

static_assert(topology::ErrorInfoTy<JpegCompressorErrorInfo>);

/**
 * @brief RAII wrapper for libjpeg compression structures.
 *
 * @details
 * `JpegCompressor` manages the lifetime of a @c jpeg_compress_struct and its
 * associated @ref JpegError handler. It ensures proper initialization and
 * cleanup of libjpeg resources.
 *
 * Copy operations are disabled via @ref Movable.
 */
class JpegCompressor : public Movable<JpegCompressor> {

public:
  /**
   * @brief Constructs and initializes the compression object.
   */
  JpegCompressor() noexcept;

  /**
   * @brief Destroys the compression object and releases resources.
   */
  ~JpegCompressor();

  tl::expected<void, JpegCompressorErrorInfo>
  init(FILE *outfp, const InputImg &inputimg, int quality) noexcept;
  tl::expected<void, JpegCompressorErrorInfo> compress() noexcept;

  /**
   * @brief Access the underlying libjpeg compression structure.
   *
   * @return Reference to @c jpeg_compress_struct.
   */
  jpeg_compress_struct &cinfo() noexcept;

  /**
   * @brief Access the associated error handler.
   *
   * @return Reference to @ref JpegError.
   */
  JpegError &err() noexcept;

private:
  /** @brief libjpeg compression structure. */
  jpeg_compress_struct cinfo_;

  /** @brief libjpeg error handler. */
  JpegError err_;
};

} // namespace img

#endif
