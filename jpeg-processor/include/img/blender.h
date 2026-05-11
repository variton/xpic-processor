
#ifndef BLENDER_H
#define BLENDER_H

#include <tuple>

#include <error_info.h>
#include <errty.h>
#include <jpeg_compressor.h>
#include <jpeg_decompressor.h>
#include <ncnm.h>
#include <tl/expected.hpp>

namespace img {

/**
 * @brief Alias for a non-copyable, non-movable base class.
 *
 * @details
 * Uses the CRTP pattern through @c core::NCNM to disable both copy and move
 * operations for the derived type.
 *
 * This is useful for types managing resources or references whose ownership
 * and lifetime must remain stable.
 *
 * @tparam T Derived type.
 */
template <typename T> using Default = core::NCNM<T>;

/**
 * @brief Blender-specific error categories.
 *
 * @details
 * Represents high-level failure states that may occur during
 * image blending and JPEG pipeline finalization.
 */
enum class BlenderError {

  /** Failure while finalizing JPEG compression. */
  BlendFinishCompressionError,

  /** Failure while finalizing JPEG decompression. */
  BlendFinishDecompressionError,
};

/**
 * @brief Structured error information for image operations.
 *
 * @details
 * Wraps a @ref BlenderError together with additional diagnostic
 * information such as contextual messages and error metadata.
 */
using BlenderErrorInfo = err::ErrorInfo<BlenderError>;

/**
 * @brief Verifies that @ref BlenderErrorInfo satisfies the required
 * error-info concept.
 */
static_assert(topology::ErrorInfoTy<BlenderErrorInfo>);

/**
 * @brief Image dimensions represented as width and height.
 *
 * @details
 * Tuple layout:
 * - index 0 → width
 * - index 1 → height
 */
using ImgDimension = std::tuple<int, int>;

/**
 * @brief Performs JPEG-based image blending operations.
 *
 * @details
 * `Blender` coordinates interaction between a
 * @ref JpegCompressor and @ref JpegDecompressor to process
 * and blend image data using the provided input image metadata.
 *
 * The class operates on externally managed compressor and decompressor
 * instances and does not own them.
 *
 * Copy and move operations are disabled via @ref Default.
 */
class Blender : public Default<Blender> {

public:

  /**
   * @brief Construct a blender for the specified input image.
   *
   * @param inputimg Reference to image metadata and input configuration.
   *
   * @details
   * Stores a reference to the provided input image descriptor for use
   * during blending operations.
   *
   * The referenced object must outlive the @ref Blender instance.
   */
  explicit Blender(const InputImg &inputimg) noexcept;

  /**
   * @brief Destroy the blender object.
   *
   * @details
   * Performs cleanup of any internally managed resources.
   */
  ~Blender();

  /**
   * @brief Blend image data using JPEG decompression and compression.
   *
   * @param compressor JPEG compressor used for output encoding.
   * @param decompressor JPEG decompressor used for input decoding.
   *
   * @return Image dimensions on success, or
   *         @ref BlenderErrorInfo on failure.
   *
   * @details
   * Coordinates the decompression, processing, and recompression
   * pipeline for image blending operations.
   *
   * The returned dimensions represent the processed image size.
   *
   * Notes:
   * - Requires properly initialized compressor and decompressor instances.
   * - The exact blending behavior depends on implementation details
   *   in the corresponding source file.
   */
  tl::expected<ImgDimension, BlenderErrorInfo>
  blend(JpegCompressor &compressor,
        JpegDecompressor &decompressor) noexcept;

private:

  /**
   * @brief Reference to the input image configuration.
   *
   * @details
   * Non-owning reference used throughout the blending process.
   * The referenced object must remain valid for the lifetime
   * of the blender instance.
   */
  const InputImg &inputimg_;
};

} // namespace img

#endif // BLENDER_H
