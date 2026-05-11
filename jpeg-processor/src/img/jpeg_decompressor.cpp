#include <jpeg_decompressor.h>

namespace img {

/**
 * Constructor
 *
 * Initializes the libjpeg decompression structure and configures
 * custom error handling.
 *
 * Notes:
 * - libjpeg requires explicit initialization via jpeg_create_decompress()
 * - The error manager (err_) must outlive cinfo_
 * - Custom error handling replaces libjpeg's default exit() behavior
 */
JpegDecompressor::JpegDecompressor() noexcept : cinfo_{}, err_{} {

  // Attach default error manager to the decompressor
  cinfo_.err = jpeg_std_error(&err_);

  // Override fatal error handler:
  // this is expected to perform longjmp instead of terminating the process
  err_.error_exit = jpeg_error_exit;

  // Allocate and initialize decompression structures
  jpeg_create_decompress(&cinfo_);
}

/**
 * Destructor
 *
 * Releases all resources associated with the decompressor.
 *
 * Important:
 * - Must be called to avoid memory leaks inside libjpeg
 * - Safe even if decompression did not complete
 */
JpegDecompressor::~JpegDecompressor() {

  // Clean up internal libjpeg allocations
  jpeg_destroy_decompress(&cinfo_);
}

/**
 * Initialize JPEG decompression.
 *
 * Configures the libjpeg input source, reads JPEG header metadata,
 * and prepares the decompressor for output conversion.
 *
 * @param infp Input file pointer containing JPEG data.
 *
 * @return Empty expected on success, or
 *         JpegDecompressorErrorInfo on failure.
 *
 * Notes:
 * - Uses setjmp/longjmp because libjpeg reports fatal errors that way.
 * - Reads JPEG metadata including dimensions and component information.
 * - Configures output conversion to YCbCr color space.
 * - Must be called before decompress() or finish_decompress().
 */
tl::expected<void, JpegDecompressorErrorInfo>
JpegDecompressor::init(FILE *infp) noexcept {

  // libjpeg uses setjmp/longjmp for error handling
  if (setjmp(err_.setjmp_buf))
    return propagate_err(err_, JpegDecompressorError::InitDecompressionError,
                         "JPEG decompression init failed");

  // Attach input file to decompressor
  jpeg_stdio_src(&cinfo_, infp);

  // Read JPEG header (metadata, dimensions, etc.)
  jpeg_read_header(&cinfo_, TRUE);

  // Force output to YCbCr:
  // libjpeg will convert automatically from source format
  // (RGB, grayscale, etc.)
  cinfo_.out_color_space = JCS_YCbCr;

  return {};
}

/**
 * Start JPEG decompression.
 *
 * Begins the libjpeg decompression process after initialization.
 * This prepares the decompressor for scanline reading.
 *
 * @return Empty expected on success, or
 *         JpegDecompressorErrorInfo on failure.
 *
 * Errors:
 * - Returns NotInitialized if init() was not successfully called.
 * - Returns DecompressionError if libjpeg reports a fatal error.
 *
 * Notes:
 * - After successful execution, scanlines may be read using
 *   jpeg_read_scanlines().
 * - libjpeg allocates additional internal buffers during this phase.
 */
tl::expected<void, JpegDecompressorErrorInfo>
JpegDecompressor::decompress() noexcept {

  // Ensure decompressor was initialized with valid image metadata
  if (!is_initialized())
    return err::unexpected(JpegDecompressorError::NotInitialized,
                           "JPEG decompression not initialized\n");

  // Install libjpeg error recovery point
  if (setjmp(err_.setjmp_buf)) {
    return propagate_err(err_, JpegDecompressorError::DecompressionError,
                         "JPEG decompression failed");
  }

  // Begin decompression stream
  jpeg_start_decompress(&cinfo_);

  return {};
}

/**
 * Finish JPEG decompression.
 *
 * Completes the decompression stream and releases temporary
 * resources allocated by libjpeg during decoding.
 *
 * @return Empty expected on success, or
 *         JpegDecompressorErrorInfo on failure.
 *
 * Notes:
 * - Should be called after all scanlines have been read.
 * - Finalizes decompression state and validates stream completion.
 * - Safe error propagation is handled through the configured setjmp buffer.
 */
tl::expected<void, JpegDecompressorErrorInfo>
JpegDecompressor::finish_decompress() noexcept {

  // Ensure decompressor has been initialized
  if (!is_initialized())
    return err::unexpected(JpegDecompressorError::NotInitialized,
                           "JPEG decompression not initialized\n");

  // Install libjpeg error recovery point
  if (setjmp(err_.setjmp_buf))
    return propagate_err(err_, JpegDecompressorError::FinishDecompressionError,
                         "JPEG finish decompression failed");

  // Complete decompression and release temporary state
  jpeg_finish_decompress(&cinfo_);

  return {};
}

/**
 * Access the underlying libjpeg decompression struct.
 *
 * @return Reference to jpeg_decompress_struct
 *
 * Usage:
 * - Required for all libjpeg API operations
 *   (reading headers, scanlines, etc.)
 * - Caller must follow libjpeg's required call sequence
 * - Allows advanced/custom libjpeg operations outside this wrapper
 */
jpeg_decompress_struct &JpegDecompressor::cinfo() noexcept {
  return cinfo_;
}

/**
 * Access the error handler structure.
 *
 * @return Reference to JpegError
 *
 * Usage:
 * - Used with setjmp/longjmp error handling pattern
 * - Caller typically sets a jump point before invoking libjpeg operations
 * - Stores libjpeg error state and jump buffer
 */
JpegError &JpegDecompressor::err() noexcept {
  return err_;
}

/**
 * Check whether the decompressor has been initialized.
 *
 * @return true if JPEG metadata has been successfully loaded;
 *         false otherwise.
 *
 * Notes:
 * - This performs a lightweight validation based on image dimensions
 *   and component count.
 * - It does not guarantee that decompression is currently active.
 * - Used internally to guard invalid decompression operations.
 */
bool JpegDecompressor::is_initialized() noexcept {
  return cinfo_.image_width > 0 &&
         cinfo_.image_height > 0 &&
         cinfo_.num_components > 0;
}

} // namespace img
