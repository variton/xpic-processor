
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
 * Access the underlying libjpeg decompression struct.
 *
 * @return Reference to jpeg_decompress_struct
 *
 * Usage:
 * - Required for all libjpeg API operations (reading headers, scanlines, etc.)
 * - Caller must follow libjpeg's required call sequence
 */
jpeg_decompress_struct &JpegDecompressor::cinfo() noexcept { return cinfo_; }

/**
 * Access the error handler structure.
 *
 * @return Reference to JpegError
 *
 * Usage:
 * - Used with setjmp/longjmp error handling pattern
 * - Caller typically sets a jump point before invoking libjpeg operations
 */
JpegError &JpegDecompressor::err() noexcept { return err_; }

} // namespace img
