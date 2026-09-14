
#ifndef MEAN_SSIM_MGR_H
#define MEAN_SSIM_MGR_H

#include <cstdint>
#include <memory>
#include <ncnm.h>
#include <span>

#include <err_utils.h>
#include <tl/expected.hpp>

namespace metric {

template <typename T> using Default = core::NCNM<T>;

/** @brief Errors produced while computing the mean SSIM. */
enum class MeanSSIMMgrError {
  MeanSSIMImgDimError, /**< Invalid image dimensions. */
  MeanSSIMStructError, /**< Invalid image data or internal structure. */
  MeanSSIMChannelError /**< Invalid number of image components. */
};

ERR_DEFINE_ERROR_INFO(MeanSSIMMgrError, MeanSSIMMgrErrorInfo);

/** @brief Computes the mean SSIM between an original and a watermarked image.
 */
class MeanSSIMMgr : public Default<MeanSSIMMgr> {
public:
  /**
   * @brief Constructs a mean SSIM manager.
   * @param original Original image data.
   * @param watermarked Watermarked image data.
   *
   * The referenced data must remain valid for the lifetime of this object.
   */
  explicit MeanSSIMMgr(std::span<const std::uint8_t> original,
                       std::span<const std::uint8_t> watermarked) noexcept;

  /** @brief Destroys the manager. */
  ~MeanSSIMMgr();

  /**
   * @brief Computes the mean SSIM.
   * @param width Image width in pixels.
   * @param height Image height in pixels.
   * @param components Number of components per pixel.
   * @return The mean SSIM value, or error information on failure.
   */
  [[nodiscard]] tl::expected<double, MeanSSIMMgrErrorInfo>
  computeMeanSSIM(int width, int height, int components) noexcept;

  /**
   * @brief Computes the mean SSIM using multiple threads.
   * @param width Image width in pixels.
   * @param height Image height in pixels.
   * @param components Number of components per pixel.
   * @return The mean SSIM value, or error information on failure.
   */
  [[nodiscard]] tl::expected<double, MeanSSIMMgrErrorInfo>
  MtComputeMeanSSIM(int width, int height, int components) noexcept;

private:
  std::span<const std::uint8_t> original_; /**< Original image data view. */
  std::span<const std::uint8_t>
      watermarked_; /**< Watermarked image data view. */
};

} // namespace metric

#endif // MEAN_SSIM_MGR_H
