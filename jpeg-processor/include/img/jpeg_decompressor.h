
#ifndef JPEG_DECOMPRESSOR_H
#define JPEG_DECOMPRESSOR_H

#include <error_info.h>
#include <errty.h>
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

enum class JpegDecompressorError { InitDecompressionError, DecompressionError };

using JpegDecompressorErrorInfo = err::ErrorInfo<JpegDecompressorError>;

static_assert(topology::ErrorInfoTy<JpegDecompressorErrorInfo>);

/**
 * @brief RAII wrapper for libjpeg decompression structures.
 *
 * @details
 * `JpegDecompressor` manages the lifetime of a @c jpeg_decompress_struct and
 * its associated @ref JpegError handler. It ensures proper initialization and
 * cleanup of libjpeg resources.
 *
 * Copy operations are disabled via @ref Movable.
 */
class JpegDecompressor : public Movable<JpegDecompressor> {
public:
  /**
   * @brief Constructs and initializes the decompression object.
   */
  JpegDecompressor() noexcept;

  /**
   * @brief Destroys the decompression object and releases resources.
   */
  ~JpegDecompressor();

  tl::expected<void, JpegDecompressorErrorInfo> init(FILE *infp) noexcept;
  tl::expected<void, JpegDecompressorErrorInfo> decompress() noexcept;

  /**
   * @brief Access the underlying libjpeg decompression structure.
   *
   * @return Reference to @c jpeg_decompress_struct.
   */
  jpeg_decompress_struct &cinfo() noexcept;

  /**
   * @brief Access the associated error handler.
   *
   * @return Reference to @ref JpegError.
   */
  JpegError &err() noexcept;

private:
  /** @brief libjpeg decompression structure. */
  jpeg_decompress_struct cinfo_;

  /** @brief libjpeg error handler. */
  JpegError err_;
};

} // namespace img

#endif
