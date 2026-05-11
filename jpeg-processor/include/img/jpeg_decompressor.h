
#ifndef JPEG_DECOMPRESSOR_H
#define JPEG_DECOMPRESSOR_H

#include <error_info.h>
#include <errty.h>
#include <jerr.h>
#include <nc.h>
#include <tl/expected.hpp>

namespace img {

/**
 * @brief Alias for a non-copyable movable base class.
 *
 * @details
 * Inherits utility semantics from @c core::NC to disable copy operations while
 * still allowing move semantics where supported.
 *
 * @tparam T Derived type using CRTP-style inheritance.
 */
template <typename T> using Movable = core::NC<T>;

/**
 * @brief JPEG decompressor error categories.
 *
 * @details
 * Represents high-level failure states that may occur during the
 * JPEG decompression lifecycle.
 */
enum class JpegDecompressorError {

  /** Decompressor was used before successful initialization. */
  NotInitialized,

  /** Failure while initializing decompression parameters or input source. */
  InitDecompressionError,

  /** Failure while starting or performing decompression. */
  DecompressionError,

  /** Failure while finalizing decompression output. */
  FinishDecompressionError
};

/**
 * @brief Error information type for JPEG decompressor operations.
 *
 * @details
 * Wraps a @ref JpegDecompressorError together with additional diagnostic
 * information such as error messages and contextual metadata.
 */
using JpegDecompressorErrorInfo =
    err::ErrorInfo<JpegDecompressorError>;

/**
 * @brief Verifies that @ref JpegDecompressorErrorInfo satisfies the expected
 * error-info concept requirements.
 */
static_assert(topology::ErrorInfoTy<JpegDecompressorErrorInfo>);

/**
 * @brief RAII wrapper for libjpeg decompression structures.
 *
 * @details
 * `JpegDecompressor` manages the lifetime of a @c jpeg_decompress_struct and
 * its associated @ref JpegError handler. It ensures proper initialization and
 * cleanup of libjpeg resources.
 *
 * The class provides a safer C++ interface around libjpeg's procedural API,
 * including:
 * - automatic resource cleanup,
 * - structured error propagation via @c tl::expected,
 * - integration with libjpeg's setjmp/longjmp error model.
 *
 * Decompression workflow:
 * 1. Construct decompressor
 * 2. Call @ref init
 * 3. Call @ref decompress
 * 4. Read scanlines through libjpeg APIs
 * 5. Call @ref finish_decompress
 *
 * Copy operations are disabled via @ref Movable.
 */
class JpegDecompressor : public Movable<JpegDecompressor> {

public:
  /**
   * @brief Constructs and initializes the decompression object.
   *
   * @details
   * Initializes libjpeg internal decompression structures and configures
   * custom error handling callbacks.
   *
   * The object is ready for configuration after construction.
   */
  JpegDecompressor() noexcept;

  /**
   * @brief Destroys the decompression object and releases resources.
   *
   * @details
   * Cleans up all memory and internal state allocated by libjpeg.
   *
   * Safe to call even if decompression was interrupted or incomplete.
   */
  ~JpegDecompressor();

  /**
   * @brief Initialize JPEG decompression parameters.
   *
   * @param infp Input file stream containing JPEG data.
   *
   * @return Empty expected on success, or
   *         @ref JpegDecompressorErrorInfo on failure.
   *
   * @details
   * Configures:
   * - decompression input source,
   * - JPEG header parsing,
   * - image metadata loading,
   * - output color space conversion.
   *
   * Must be called before @ref decompress.
   */
  tl::expected<void, JpegDecompressorErrorInfo>
  init(FILE *infp) noexcept;

  /**
   * @brief Start JPEG decompression.
   *
   * @return Empty expected on success, or
   *         @ref JpegDecompressorErrorInfo on failure.
   *
   * @details
   * Begins the libjpeg decompression process and prepares the decompressor
   * for scanline reading via libjpeg APIs such as
   * @c jpeg_read_scanlines().
   *
   * Requires successful initialization through @ref init.
   */
  tl::expected<void, JpegDecompressorErrorInfo>
  decompress() noexcept;

  /**
   * @brief Finalize JPEG decompression.
   *
   * @return Empty expected on success, or
   *         @ref JpegDecompressorErrorInfo on failure.
   *
   * @details
   * Flushes remaining decompression state, validates stream completion,
   * and releases temporary decoding resources.
   *
   * Should be called after all scanlines have been read.
   */
  tl::expected<void, JpegDecompressorErrorInfo>
  finish_decompress() noexcept;

  /**
   * @brief Access the underlying libjpeg decompression structure.
   *
   * @return Reference to @c jpeg_decompress_struct.
   *
   * @details
   * Provides direct access to libjpeg internals for advanced configuration
   * or scanline operations not wrapped by this class.
   *
   * Caller must respect libjpeg lifecycle rules.
   */
  jpeg_decompress_struct &cinfo() noexcept;

  /**
   * @brief Access the associated error handler.
   *
   * @return Reference to @ref JpegError.
   *
   * @details
   * Exposes the custom error structure used for libjpeg setjmp/longjmp
   * error handling.
   */
  JpegError &err() noexcept;

private:

  /**
   * @brief Check whether the decompressor has valid initialization state.
   *
   * @return true if decompression parameters were initialized;
   *         false otherwise.
   *
   * @details
   * Performs a lightweight validation of image dimensions and component
   * configuration loaded from the JPEG header.
   */
  bool is_initialized() noexcept;

  /** @brief libjpeg decompression structure. */
  jpeg_decompress_struct cinfo_;

  /**
   * @brief libjpeg error handler structure.
   *
   * @details
   * Stores libjpeg error callbacks and setjmp state used for
   * fatal error recovery.
   */
  JpegError err_;
};

} // namespace img

#endif
