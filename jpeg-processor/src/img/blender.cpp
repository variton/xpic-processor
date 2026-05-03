
#include <blender.h>
#include <vector>

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

Blender::Blender(const InputImg &inputimg) noexcept : inputimg_{inputimg} {}

Blender::~Blender() = default;

tl::expected<ImgDimension, BlenderErrorInfo>
Blender::blend(JpegCompressor &compressor,
               JpegDecompressor &decompressor) noexcept {
  ::Row prev_row(inputimg_.row_stride);
  ::Row curr_row(inputimg_.row_stride);

  for (int line = 0; line < inputimg_.height; ++line) {
    // Read one scanline from decompressor
    JSAMPROW ptr = curr_row.data();
    jpeg_read_scanlines(&decompressor.cinfo(), &ptr, 1);

    // First line is written as-is (no previous row to blend with)
    // Subsequent lines are blended with the previous one
    const ::Row &out_row =
        (line == 0) ? curr_row : ::blend_rows(curr_row, prev_row);

    // Write processed scanline to compressor
    JSAMPROW out_ptr = const_cast<JSAMPROW>(out_row.data());
    jpeg_write_scanlines(&compressor.cinfo(), &out_ptr, 1);

    // Store current row for next iteration
    prev_row = curr_row;
  }

  // Finalize decompression and compression
  auto ret_finish_decompress = decompressor.finish_decompress();
  if (!ret_finish_decompress)
    return err::unexpected(BlenderError::BlendFinishDecompressionError,
                           ret_finish_decompress.error().message);

  // return tl::unexpected(
  //     BlenderErrorInfo{BlenderError::BlendFinishDecompressionError,
  //                      ret_finish_decompress.error().message});

  auto ret_finish_compress = compressor.finish_compress();
  if (!ret_finish_compress)
    return tl::unexpected(
        BlenderErrorInfo{BlenderError::BlendFinishCompressionError,
                         ret_finish_compress.error().message});
  return ImgDimension{inputimg_.width, inputimg_.height};
}

} // namespace img
