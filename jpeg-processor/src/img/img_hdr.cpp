#include <vector>

#include <filehandler.h>
#include <img_hdr.h>
#include <inputimg.h>
#include <jpeg_compressor.h>
#include <jpeg_decompressor.h>

namespace {

// Alias for a single image row (scanline)
using Row = std::vector<JSAMPLE>;

/**
 * Blend two scanlines channel-by-channel.
 *
 * Each pixel component is averaged:
 *   out[i] = (curr[i] + prev[i]) / 2
 *
 * Notes:
 * - JSAMPLE is typically an unsigned byte (0–255).
 * - We cast to int before addition to avoid overflow.
 * - Assumes both rows have identical size.
 */
static Row blend_rows(const Row &curr, const Row &prev) {
  Row out(curr.size());

  for (std::size_t i = 0; i < curr.size(); ++i) {
    out[i] = static_cast<JSAMPLE>(
        (static_cast<int>(curr[i]) + static_cast<int>(prev[i])) / 2);
  }

  return out;
}

} // namespace

namespace img {

/**
 * Constructor
 *
 * Stores input/output paths and sets default JPEG quality.
 */
ImgHdr::ImgHdr(std::string_view input, std::string_view output) noexcept
    : input_{input}, output_{output}, quality_{85} {}

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

  // -----------------------------
  // Decompressor setup & processing
  // -----------------------------
  JpegDecompressor dec{};
  auto ret_init_decompressor = dec.init(infp.get());
  if (!ret_init_decompressor)
    return tl::unexpected(ImageErrorInfo{
        ImageError::DecodingError, ret_init_decompressor.error().message});

  auto ret_decompress = dec.decompress();
  if (!ret_decompress)
    return tl::unexpected(ImageErrorInfo{ImageError::DecodingError,
                                         ret_decompress.error().message});

  InputImg inputimg{dec.cinfo()};
  // -----------------------------
  // Compressor setup & processing
  // -----------------------------
  JpegCompressor enc{};

  auto ret_init_compressor = quality > 0
                                 ? enc.init(outfp.get(), inputimg, quality)
                                 : enc.init(outfp.get(), inputimg, quality_);

  if (!ret_init_compressor)
    return tl::unexpected(ImageErrorInfo{ImageError::EncodingError,
                                         ret_init_compressor.error().message});

  auto ret_compression = enc.compress();

  if (!ret_compression)
    return tl::unexpected(ImageErrorInfo{ImageError::EncodingError,
                                         ret_compression.error().message});
  // -----------------------------
  // Generation of deinterlaced output
  // -----------------------------

  // Buffers for current and previous scanlines
  std::vector<JSAMPLE> prev_row(inputimg.row_stride);
  std::vector<JSAMPLE> curr_row(inputimg.row_stride);

  for (int line = 0; line < inputimg.height; ++line) {
    // Read one scanline from decompressor
    JSAMPROW ptr = curr_row.data();
    jpeg_read_scanlines(&dec.cinfo(), &ptr, 1);

    // First line is written as-is (no previous row to blend with)
    // Subsequent lines are blended with the previous one
    const Row &out_row =
        (line == 0) ? curr_row : ::blend_rows(curr_row, prev_row);

    // Write processed scanline to compressor
    JSAMPROW out_ptr = const_cast<JSAMPROW>(out_row.data());
    jpeg_write_scanlines(&enc.cinfo(), &out_ptr, 1);

    // Store current row for next iteration
    prev_row = curr_row;
  }

  // Finalize decompression and compression
  jpeg_finish_decompress(&dec.cinfo());
  jpeg_finish_compress(&enc.cinfo());

  // Return resulting image dimensions
  return ImgDimension{inputimg.width, inputimg.height};
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
