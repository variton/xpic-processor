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

} // namespace img
