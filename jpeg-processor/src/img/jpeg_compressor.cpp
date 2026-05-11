#include <jpeg_compressor.h>

namespace img {

/**
 * Constructor
 *
 * Initializes the libjpeg compression structure and sets up
 * custom error handling.
 *
 * Notes:
 * - libjpeg requires explicit initialization via jpeg_create_compress()
 * - Error handling is configured before initialization
 * - err_ must remain valid for the lifetime of cinfo_
 */
JpegCompressor::JpegCompressor() noexcept : cinfo_{}, err_{} {

  // Initialize default error manager and attach it to cinfo_
  cinfo_.err = jpeg_std_error(&err_);

  // Override default error handler:
  // libjpeg will call this on fatal errors (typically triggers longjmp)
  err_.error_exit = jpeg_error_exit;

  // Allocate and initialize internal compression structures
  jpeg_create_compress(&cinfo_);
}

/**
 * Destructor
 *
 * Cleans up all resources allocated by libjpeg.
 *
 * Important:
 * - Must always be called to avoid memory leaks
 * - Safe to call even if compression was not fully completed
 */
JpegCompressor::~JpegCompressor() {
  // Release all resources associated with cinfo_
  jpeg_destroy_compress(&cinfo_);
}

/**
 * Initialize JPEG compression.
 *
 * Configures the libjpeg destination, image dimensions, component count,
 * input color space, default compression parameters, and output quality.
 *
 * @param outfp Output file pointer used as the JPEG destination.
 * @param inputimg Input image metadata used to configure compression.
 * @param quality JPEG quality value passed to libjpeg.
 *
 * @return Empty expected on success, or JpegCompressorErrorInfo on failure.
 *
 * Notes:
 * - Uses setjmp/longjmp because libjpeg reports fatal errors that way.
 * - Assumes input data is already in YCbCr format.
 * - Must be called before compress() or finish_compress().
 */
tl::expected<void, JpegCompressorErrorInfo>
JpegCompressor::init(FILE *outfp, const InputImg &inputimg,
                     int quality) noexcept {

  // libjpeg uses setjmp/longjmp for error handling
  if (setjmp(err_.setjmp_buf))
    return propagate_err(err_, JpegCompressorError::InitCompressionError,
                         "JPEG compression init failed");

  // Configure libjpeg to write compressed data to the output FILE*
  jpeg_stdio_dest(&cinfo_, outfp);

  // Configure output image parameters
  cinfo_.image_width = static_cast<JDIMENSION>(inputimg.width);
  cinfo_.image_height = static_cast<JDIMENSION>(inputimg.height);
  cinfo_.input_components = inputimg.components;

  // We already provide YCbCr data → no conversion needed
  cinfo_.in_color_space = JCS_YCbCr;

  // Set default compression parameters
  jpeg_set_defaults(&cinfo_);

  // Configure compression quality
  // TRUE limits values to baseline-JPEG compatible ranges
  jpeg_set_quality(&cinfo_, quality, TRUE);

  return {};
}

/**
 * Start JPEG compression.
 *
 * Begins the libjpeg compression process after the compressor has been
 * initialized. This must be called before writing scanlines.
 *
 * @return Empty expected on success, or JpegCompressorErrorInfo on failure.
 *
 * Errors:
 * - Returns NotInitialized if init() has not configured valid image metadata.
 * - Returns CompressionError if libjpeg reports a fatal compression error.
 *
 * Notes:
 * - After successful execution, scanlines may be written using
 *   jpeg_write_scanlines().
 * - libjpeg allocates additional internal state during this phase.
 */
tl::expected<void, JpegCompressorErrorInfo>
JpegCompressor::compress() noexcept {

  // Ensure compressor was initialized with valid image metadata
  if (!is_initialized())
    return err::unexpected(JpegCompressorError::NotInitialized,
                           "JPEG compression not initialized\n");

  // Install libjpeg error recovery point
  if (setjmp(err_.setjmp_buf))
    return propagate_err(err_, JpegCompressorError::CompressionError,
                         "JPEG compression failed");

  // Begin compression stream
  jpeg_start_compress(&cinfo_, TRUE);

  return {};
}

/**
 * Finish JPEG compression.
 *
 * Completes the compression stream and flushes any remaining output data
 * managed by libjpeg.
 *
 * @return Empty expected on success, or JpegCompressorErrorInfo on failure.
 *
 * Notes:
 * - Should be called after all scanlines have been written.
 * - Finalizes internal JPEG structures and writes EOI markers.
 * - Safe error propagation is handled through the configured setjmp buffer.
 */
tl::expected<void, JpegCompressorErrorInfo>
JpegCompressor::finish_compress() noexcept {

  // Ensure compressor has been initialized
  if (!is_initialized())
    return err::unexpected(JpegCompressorError::NotInitialized,
                           "JPEG compression not initialized\n");

  // Install libjpeg error recovery point
  if (setjmp(err_.setjmp_buf))
    return propagate_err(err_, JpegCompressorError::FinishCompressionError,
                         "JPEG finish compression failed");

  // Complete compression and flush remaining output
  jpeg_finish_compress(&cinfo_);

  return {};
}

/**
 * Access the underlying libjpeg compression struct.
 *
 * @return Reference to jpeg_compress_struct
 *
 * Usage:
 * - Required for all libjpeg API calls (configuration, writing, etc.)
 * - Caller must respect libjpeg lifecycle rules
 * - Allows advanced/custom libjpeg operations outside this wrapper
 */
jpeg_compress_struct &JpegCompressor::cinfo() noexcept { return cinfo_; }

/**
 * Access the error handler structure.
 *
 * @return Reference to JpegError
 *
 * Usage:
 * - Needed for setjmp/longjmp error handling pattern used by libjpeg
 * - Caller typically installs setjmp before performing operations
 * - Stores libjpeg error state and jump buffer
 */
JpegError &JpegCompressor::err() noexcept { return err_; }

/**
 * Check whether the compressor has been initialized.
 *
 * @return true if required image metadata has been configured;
 *         false otherwise.
 *
 * Notes:
 * - This is a lightweight state check based on image dimensions and component
 *   count.
 * - It does not guarantee that libjpeg is currently in a valid compression
 *   phase for every operation.
 * - Used internally to prevent invalid compression calls.
 */
bool JpegCompressor::is_initialized() noexcept {
  return cinfo_.image_width > 0 && cinfo_.image_height > 0 &&
         cinfo_.input_components > 0;
}

} // namespace img
