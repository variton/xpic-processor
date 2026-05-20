
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
 * @brief JPEG compressor error categories.
 *
 * @details
 * Represents high-level failure states that may occur during the
 * JPEG compression lifecycle.
 */
enum class JpegCompressorError {

  /** Compressor was used before successful initialization. */
  NotInitialized,

  /** Failure while initializing compression parameters or destination. */
  InitCompressionError,

  /** Failure while starting or performing compression. */
  CompressionError,

  /** Failure while finalizing compression output. */
  FinishCompressionError,
};

/**
 * @brief Error information type for JPEG compressor operations.
 *
 * @details
 * Wraps a @ref JpegCompressorError together with additional diagnostic
 * information such as error messages and contextual metadata.
 */
using JpegCompressorErrorInfo = err::ErrorInfo<JpegCompressorError>;

/**
 * @brief Verifies that @ref JpegCompressorErrorInfo satisfies the expected
 * error-info concept requirements.
 */
static_assert(topology::ErrorInfoTy<JpegCompressorErrorInfo>);

/**
 * @brief RAII wrapper for libjpeg compression structures.
 *
 * @details
 * `JpegCompressor` manages the lifetime of a @c jpeg_compress_struct and its
 * associated @ref JpegError handler. It ensures proper initialization and
 * cleanup of libjpeg resources.
 *
 * The class provides a safer C++ interface around libjpeg's procedural API,
 * including:
 * - automatic resource cleanup,
 * - structured error propagation via @c tl::expected,
 * - integration with libjpeg's setjmp/longjmp error model.
 *
 * Compression workflow:
 * 1. Construct compressor
 * 2. Call @ref init
 * 3. Call @ref compress
 * 4. Write scanlines through libjpeg APIs
 * 5. Call @ref finish_compress
 *
 * Copy operations are disabled via @ref Movable.
 */
class JpegCompressor : public Movable<JpegCompressor> {

public:
  /**
   * @brief Constructs and initializes the compression object.
   *
   * @details
   * Initializes libjpeg internal compression structures and configures
   * custom error handling callbacks.
   *
   * The object is ready for configuration after construction.
   */
  JpegCompressor() noexcept;

  /**
   * @brief Destroys the compression object and releases resources.
   *
   * @details
   * Cleans up all memory and internal state allocated by libjpeg.
   *
   * Safe to call even if compression was interrupted or incomplete.
   */
  ~JpegCompressor();

  /**
   * @brief Initialize JPEG compression parameters.
   *
   * @param outfp Output file stream receiving compressed JPEG data.
   * @param inputimg Input image metadata used for compression setup.
   * @param quality JPEG quality value passed to libjpeg.
   *
   * @return Empty expected on success, or
   *         @ref JpegCompressorErrorInfo on failure.
   *
   * @details
   * Configures:
   * - compression destination,
   * - image dimensions,
   * - component count,
   * - color space,
   * - default JPEG settings,
   * - quality level.
   *
   * Must be called before @ref compress.
   */
  tl::expected<void, JpegCompressorErrorInfo>
  init(FILE *outfp, const InputImg &inputimg, int quality) noexcept;

  /**
   * @brief Start JPEG compression.
   *
   * @return Empty expected on success, or
   *         @ref JpegCompressorErrorInfo on failure.
   *
   * @details
   * Begins the libjpeg compression process and prepares the compressor
   * for scanline writing via libjpeg APIs such as
   * @c jpeg_write_scanlines().
   *
   * Requires successful initialization through @ref init.
   */
  tl::expected<void, JpegCompressorErrorInfo> compress() noexcept;

  /**
   * @brief Finalize JPEG compression.
   *
   * @return Empty expected on success, or
   *         @ref JpegCompressorErrorInfo on failure.
   *
   * @details
   * Flushes pending compressed data, writes JPEG end markers,
   * and finalizes the compression stream.
   *
   * Should be called after all scanlines have been written.
   */
  tl::expected<void, JpegCompressorErrorInfo> finish_compress() noexcept;

  /**
   * @brief Access the underlying libjpeg compression structure.
   *
   * @return Reference to @c jpeg_compress_struct.
   *
   * @details
   * Provides direct access to libjpeg internals for advanced configuration
   * or scanline operations not wrapped by this class.
   *
   * Caller must respect libjpeg lifecycle rules.
   */
  jpeg_compress_struct &cinfo() noexcept;

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
   * @brief Check whether the compressor has valid initialization state.
   *
   * @return true if compression parameters were initialized;
   *         false otherwise.
   *
   * @details
   * Performs a lightweight validation of image dimensions and component
   * configuration.
   */
  bool is_initialized() noexcept;

  /** @brief libjpeg compression structure. */
  jpeg_compress_struct cinfo_;

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
