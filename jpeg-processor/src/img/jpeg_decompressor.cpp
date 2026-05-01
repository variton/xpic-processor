
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

tl::expected<void, JpegDecompressorErrorInfo>
JpegDecompressor::init(FILE *infp) noexcept {
  // libjpeg uses setjmp/longjmp for error handling
  if (setjmp(err_.setjmp_buf))
    return tl::unexpected(JpegDecompressorErrorInfo{JpegDecompressorError::InitDecompressionError,
                                                    "JPEG decompression init failed"});
        //std::string("JPEG decompression init failed : ") + err_.message});
        //"JPEG decompression init failed : " + std::string{err_.message}});

  // Attach input file to decompressor
  jpeg_stdio_src(&cinfo_, infp);

  // Read JPEG header (metadata, dimensions, etc.)
  jpeg_read_header(&cinfo_, TRUE);

  // Force output to YCbCr:
  // libjpeg will convert automatically from source format (RGB, grayscale,
  // etc.)
  cinfo_.out_color_space = JCS_YCbCr;

  // use to force libjpeg to emit an error message
  //jpeg_start_decompress(&cinfo_);

  return {};
}

tl::expected<void, JpegDecompressorErrorInfo>
JpegDecompressor::decompress() noexcept {

  jpeg_start_decompress(&cinfo_);

  if (setjmp(err_.setjmp_buf))
    return tl::unexpected(
        JpegDecompressorErrorInfo{JpegDecompressorError::DecompressionError,
                                  "JPEG decompression failed"});
  return {};
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
