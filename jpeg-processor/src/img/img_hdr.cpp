#include <vector>

#include <blender.h>
#include <filehandler.h>
#include <img_hdr.h>
#include <inputimg.h>
#include <jpeg_compressor.h>
#include <jpeg_decompressor.h>

namespace img {

/**
 * Constructor
 *
 * Stores input/output paths and sets default JPEG quality.
 */
ImgHdr::ImgHdr(std::string_view input, std::string_view output) noexcept
    : input_{input}, output_{output} {}

/**
 * Perform HDR-like blending by vertically averaging adjacent scanlines.
 *
 * Workflow:
 * 1. Open input/output files
 * 2. Decompress JPEG into YCbCr color space
 * 3. Blend each row with the previous one (simple deinterlacing effect)
 * 4. Compress and write result as JPEG
 *
 * @param quality Optional JPEG quality override
 * @return Image dimensions on success, or error info on failure
 */
tl::expected<ImgDimension, ImageErrorInfo> ImgHdr::blend(int quality) noexcept {
  // Initialize file handlers (RAII wrappers)
  auto handlers = init_handlers();
  if (!handlers) {
    return tl::unexpected(handlers.error());
  }
  auto [infp, outfp] = std::move(handlers.value());

  // Decompressor setup & processing
  JpegDecompressor dec{};
  auto ret_init_decompressor = dec.init(infp.get());
  if (!ret_init_decompressor)
    return err::unexpected(ImageError::DecodingError,
                           ret_init_decompressor.error().message);

  auto ret_decompress = dec.decompress();
  if (!ret_decompress)
    return err::unexpected(ImageError::DecodingError,
                           ret_decompress.error().message);
  // Input image setup
  InputImg inputimg{dec.cinfo()};

  // Compressor setup & processing
  JpegCompressor enc{};

  auto ret_init_compressor = enc.init(outfp.get(), inputimg, quality);

  if (!ret_init_compressor)
    return err::unexpected(ImageError::EncodingError,
                           ret_init_compressor.error().message);

  auto ret_compression = enc.compress();

  if (!ret_compression)
    return err::unexpected(ImageError::EncodingError,
                           ret_compression.error().message);

  // Generation of deinterlaced output
  Blender blender{inputimg};

  auto ret_blend = blender.blend(enc, dec);

  if (!ret_blend)
    return err::unexpected(ImageError::BlendError, ret_blend.error().message);
  return ret_blend.value();
}

/**
 * Initialize input/output file handlers.
 *
 * Opens files in binary mode and transfers ownership into FHandlers.
 *
 * @return Pair of file handlers or error info
 */
tl::expected<FHandlers, ImageErrorInfo> ImgHdr::init_handlers() noexcept {
  // Open input file (read-binary)
  auto input_file = fio::FileHandler::open(input_.data(), "rb");

  // Open output file (write-binary)
  auto output_file = fio::FileHandler::open(output_.data(), "wb");

  // Validate input file
  if (!input_file) {
    return tl::unexpected(ImageErrorInfo{ImageError::OpenFileError,
                                         "Failed to open input file\n"});
  }

  // Validate output file
  if (!output_file) {
    return tl::unexpected(ImageErrorInfo{ImageError::OpenFileError,
                                         "Failed to open output file\n"});
  }

  // Transfer ownership of FILE* into FHandlers
  return FHandlers{std::move(input_file).value().release(),
                   std::move(output_file).value().release()};
}

} // namespace img
