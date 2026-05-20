
#include <blender.h>
#include <vector>

namespace {

/**
 * @brief Alias representing a single image scanline.
 *
 * @details
 * Each element stores one image component value of type @c JSAMPLE,
 * typically an unsigned 8-bit channel value in the range [0, 255].
 *
 * A row contains:
 * - width × number_of_components entries
 * - contiguous pixel component data
 */
using Row = std::vector<JSAMPLE>;

/**
 * @brief Blend two scanlines channel-by-channel.
 *
 * @param curr Current image row.
 * @param prev Previous image row.
 *
 * @return Blended output row.
 *
 * @details
 * Each pixel component is averaged independently:
 *
 * @code
 * out[i] = (curr[i] + prev[i]) / 2
 * @endcode
 *
 * This produces a simple vertical blending effect between adjacent rows.
 *
 * Notes:
 * - @c JSAMPLE is typically an unsigned byte in the range [0, 255].
 * - Values are promoted to @c int before addition to avoid overflow.
 * - Both rows are expected to have identical size.
 * - The operation is performed component-wise for all color channels.
 */
static Row blend_rows(const Row &curr, const Row &prev) {

  // Allocate output row with the same number of components
  Row out(curr.size());

  // Blend each component independently
  for (std::size_t i = 0; i < curr.size(); ++i) {

    // Promote to int before addition to avoid unsigned overflow
    out[i] = static_cast<JSAMPLE>(
        (static_cast<int>(curr[i]) + static_cast<int>(prev[i])) / 2);
  }

  return out;
}

} // namespace

namespace img {

/**
 * @brief Construct a blender for the provided input image.
 *
 * @param inputimg Input image metadata and configuration.
 *
 * @details
 * Stores a non-owning reference to the input image descriptor used
 * during blending operations.
 *
 * The referenced object must outlive the blender instance.
 */
Blender::Blender(const InputImg &inputimg) noexcept : inputimg_{inputimg} {}

/**
 * @brief Destroy the blender object.
 *
 * @details
 * Uses the compiler-generated default destructor since no explicit
 * resource ownership is managed by this class.
 */
Blender::~Blender() = default;

/**
 * @brief Blend decompressed image scanlines and recompress the result.
 *
 * @param compressor JPEG compressor used for writing blended output.
 * @param decompressor JPEG decompressor used for reading source scanlines.
 *
 * @return Processed image dimensions on success, or
 *         @ref BlenderErrorInfo on failure.
 *
 * @details
 * The blending pipeline performs the following steps:
 * 1. Read scanlines from the JPEG decompressor.
 * 2. Blend each scanline with the previous one.
 * 3. Write the resulting scanline to the compressor.
 * 4. Finalize decompression and compression streams.
 *
 * Blending behavior:
 * - The first scanline is written unchanged.
 * - Each subsequent scanline is averaged with the previous scanline.
 *
 * This creates a simple vertical smoothing/blending effect.
 *
 * Notes:
 * - Assumes compressor and decompressor are already initialized
 *   and actively processing JPEG streams.
 * - Uses libjpeg scanline APIs:
 *   - @c jpeg_read_scanlines()
 *   - @c jpeg_write_scanlines()
 * - Returns output image width and height on success.
 */
tl::expected<ImgDimension, BlenderErrorInfo>
Blender::blend(JpegCompressor &compressor,
               JpegDecompressor &decompressor) noexcept {

  // Previous scanline storage
  ::Row prev_row(inputimg_.row_stride);

  // Current scanline storage
  ::Row curr_row(inputimg_.row_stride);

  // Process each image row sequentially
  for (int line = 0; line < inputimg_.height; ++line) {

    // Read one scanline from decompressor
    JSAMPROW ptr = curr_row.data();
    jpeg_read_scanlines(&decompressor.cinfo(), &ptr, 1);

    // First line is written unchanged because there is no
    // previous row available for blending.
    //
    // Remaining lines are blended with the previous scanline.
    const ::Row &out_row =
        (line == 0) ? curr_row : ::blend_rows(curr_row, prev_row);

    // Write processed scanline to compressor
    JSAMPROW out_ptr = const_cast<JSAMPROW>(out_row.data());

    jpeg_write_scanlines(&compressor.cinfo(), &out_ptr, 1);

    // Preserve current row for next iteration
    prev_row = curr_row;
  }

  // Finalize JPEG decompression
  auto ret_finish_decompress = decompressor.finish_decompress();

  if (!ret_finish_decompress)
    return err::unexpected(BlenderError::BlendFinishDecompressionError,
                           ret_finish_decompress.error().message);

  // Finalize JPEG compression
  auto ret_finish_compress = compressor.finish_compress();

  if (!ret_finish_compress)
    return err::unexpected(BlenderError::BlendFinishCompressionError,
                           ret_finish_compress.error().message);

  // Return processed image dimensions
  return ImgDimension{inputimg_.width, inputimg_.height};
}

} // namespace img
