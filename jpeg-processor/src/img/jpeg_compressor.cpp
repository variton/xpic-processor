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

tl::expected<void, JpegCompressorErrorInfo>
JpegCompressor::init(FILE *outfp, const InputImg &inputimg,
                     int quality) noexcept {
  // libjpeg uses setjmp/longjmp for error handling
  if (setjmp(err_.setjmp_buf))
    return tl::unexpected(
        JpegCompressorErrorInfo{JpegCompressorError::InitCompressionError,
                                "JPEG compression init failed\n"});

  jpeg_stdio_dest(&cinfo_, outfp);

  // Configure output image parameters
  cinfo_.image_width = static_cast<JDIMENSION>(inputimg.width);
  cinfo_.image_height = static_cast<JDIMENSION>(inputimg.height);
  cinfo_.input_components = inputimg.components;

  // We already provide YCbCr data → no conversion needed
  cinfo_.in_color_space = JCS_YCbCr;

  // Set default compression parameters
  jpeg_set_defaults(&cinfo_);

  // Use provided quality if valid, otherwise fallback to default member value
  jpeg_set_quality(&cinfo_, quality, TRUE);
  return {};
}

tl::expected<void, JpegCompressorErrorInfo>
JpegCompressor::compress() noexcept {

  if (!is_initialized())
    return err::unexpected(JpegCompressorError::NotInitialized,
                           "JPEG compression not initialized\n");

  if (setjmp(err_.setjmp_buf))
    return tl::unexpected(JpegCompressorErrorInfo{
        JpegCompressorError::CompressionError, "JPEG compression failed\n"});

  jpeg_start_compress(&cinfo_, TRUE);

  return {};
}

tl::expected<void, JpegCompressorErrorInfo>
JpegCompressor::finish_compress() noexcept {

  if (!is_initialized())
    return err::unexpected(JpegCompressorError::NotInitialized,
                           "JPEG compression not initialized\n");

  if (setjmp(err_.setjmp_buf))
    return tl::unexpected(
        JpegCompressorErrorInfo{JpegCompressorError::FinishCompressionError,
                                "JPEG finish compression failed\n"});

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
 */
JpegError &JpegCompressor::err() noexcept { return err_; }

bool JpegCompressor::is_initialized() noexcept {
  return cinfo_.image_width > 0 && cinfo_.image_height > 0 &&
         cinfo_.input_components > 0;
}

} // namespace img
