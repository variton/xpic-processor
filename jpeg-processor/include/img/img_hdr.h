
#ifndef IMG_HDR_H
#define IMG_HDR_H

#include <tuple>

#include <error_info.h>
#include <errty.h>
#include <filehandler.h>
#include <ncnm.h>
#include <tl/expected.hpp>

namespace img {

/**
 * @brief Alias for a non-copyable, non-movable base class.
 *
 * Uses the CRTP pattern to disable copy and move operations for the derived
 * type.
 *
 * @tparam T Derived type.
 */
template <typename T> using Default = core::NCNM<T>;

/**
 * @brief Image dimensions represented as width and height.
 */
using ImgDimension = std::tuple<int, int>;

/**
 * @brief Pair of input and output file handlers.
 */
using FHandlers = std::tuple<fio::FilePtr, fio::FilePtr>;

/**
 * @brief Error codes produced by @ref ImgHdr.
 */
enum class ImageError {
  /** @brief Image decoding failed. */
  DecodingError,
  /** @brief Image encoding failed. */
  EncodingError,
  /** @brief Opening an input or output file failed. */
  OpenFileError
};

/**
 * @brief Structured error information for image operations.
 */
using ImageErrorInfo = err::ErrorInfo<ImageError>;

static_assert(topology::ErrorInfoTy<ImageErrorInfo>);

/**
 * @brief HDR image processor with exclusive ownership semantics.
 *
 * @details
 * `ImgHdr` manages an input and output image path and provides a blending
 * operation that returns the resulting image dimensions on success.
 */
class ImgHdr : public Default<ImgHdr> {

public:
  /**
   * @brief Constructs an image processor for the given input and output paths.
   *
   * @param input Input image path.
   * @param output Output image path.
   */
  explicit ImgHdr(std::string_view input, std::string_view output) noexcept;

  /**
   * @brief Processes the image and writes the result.
   *
   * @param quality Encoding or processing quality parameter.
   * @return `tl::expected` containing the output image dimensions on success,
   *         or @ref ImageErrorInfo on failure.
   */
  tl::expected<ImgDimension, ImageErrorInfo> blend(int quality = 0) noexcept;

private:
  /**
   * @brief Opens and initializes the input and output file handlers.
   *
   * @return `tl::expected` containing the initialized file handlers on success,
   *         or @ref ImageErrorInfo on failure.
   */
  tl::expected<FHandlers, ImageErrorInfo> init_handlers() noexcept;

  /** @brief Input image path. */
  std::string_view input_;

  /** @brief Output image path. */
  std::string_view output_;

  /** @brief Processing quality parameter. */
  int quality_;
};

} // namespace img

#endif
