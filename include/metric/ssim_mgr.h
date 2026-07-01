/**
 * @file ssim_mgr.h
 * @brief Declares a manager for computing SSIM between two image buffers.
 */

#ifndef SSIM_MGR_H
#define SSIM_MGR_H

#include <cstdint>
#include <ncnm.h>
#include <span>

#include <error_info.h>
#include <errty.h>
#include <tl/expected.hpp>

namespace metric {

/**
 * @brief Default CRTP base alias for metric managers.
 */
template <typename T> using Default = core::NCNM<T>;

/**
 * @brief Errors that may occur while computing SSIM.
 */
enum class SSIMMgrError {
  LowValError, /**< Computed or input value is below the supported range. */
};

/**
 * @brief Error information type used by SSIMMgr.
 */
using SSIMMgrErrorInfo = err::ErrorInfo<SSIMMgrError>;

static_assert(topology::ErrorInfoTy<SSIMMgrErrorInfo>);

/**
 * @brief Computes Structural Similarity Index Measure between two image buffers.
 *
 * SSIMMgr stores non-owning views of the original and watermarked image data.
 * The referenced buffers must remain valid for the lifetime of the manager.
 */
class SSIMMgr : public Default<SSIMMgr> {

public:
  /**
   * @brief Constructs an SSIM manager from two image buffers.
   * @param original Original/reference image data.
   * @param watermarked Watermarked or modified image data.
   * @param window SSIM evaluation window size.
   * @param k1 First SSIM stability constant factor.
   * @param k2 Second SSIM stability constant factor.
   * @param l Dynamic range of the pixel values.
   */
  explicit SSIMMgr(std::span<const std::uint8_t> original,
                   std::span<const std::uint8_t> watermarked,
                   std::size_t window = 11,
                   double k1 = 0.01,
                   double k2 = 0.03,
                   double l = 255.0) noexcept;

  /**
   * @brief Destroys the SSIM manager.
   */
  ~SSIMMgr();

  /**
   * @brief Computes SSIM for the stored image buffers.
   * @param width Image width in pixels.
   * @param height Image height in pixels.
   * @return SSIM value, or SSIMMgrErrorInfo on failure.
   */
  tl::expected<double, SSIMMgrErrorInfo> computeSSIM(int width, int height) noexcept;

private:
  std::span<const std::uint8_t> original_;    /**< Original image data view. */
  std::span<const std::uint8_t> watermarked_; /**< Watermarked image data view. */
  std::size_t window_;
  double k1_;
  double k2_;
  double l_;

};

} // namespace metric

#endif
