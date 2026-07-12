/**
 * @file psnr_mgr.h
 * @brief Declares a manager for computing PSNR between two image buffers.
 */

#ifndef PSNR_MGR_H
#define PSNR_MGR_H

#include <cstdint>
#include <ncnm.h>
#include <span>

#include <err_utils.h>
#include <tl/expected.hpp>

namespace metric {

/**
 * @brief Default CRTP base alias for metric managers.
 */
template <typename T> using Default = core::NCNM<T>;

/**
 * @brief Errors that may occur while computing PSNR.
 */
enum class PSNRMgrError {
  SSENullError, /**< SSE computation returned or encountered a null state. */
};

ERR_DEFINE_ERROR_INFO(PSNRMgrError, PSNRMgrErrorInfo);
/**
 * @brief Computes Peak Signal-to-Noise Ratio between two image buffers.
 *
 * PSNRMgr stores non-owning views of the original and watermarked image data.
 * The referenced buffers must remain valid for the lifetime of the manager.
 */
class PSNRMgr : public Default<PSNRMgr> {

public:
  /**
   * @brief Constructs a PSNR manager from two image buffers.
   * @param original Original/reference image data.
   * @param watermarked Watermarked or modified image data.
   */
  explicit PSNRMgr(std::span<const std::uint8_t> original,
                   std::span<const std::uint8_t> watermarked) noexcept;

  /**
   * @brief Destroys the PSNR manager.
   */
  ~PSNRMgr();

  /**
   * @brief Computes PSNR for the stored image buffers.
   * @param width Image width in pixels.
   * @param height Image height in pixels.
   * @param channels Number of channels per pixel.
   * @return PSNR value, or PSNRMgrErrorInfo on failure.
   */
  tl::expected<double, PSNRMgrErrorInfo> computePNSR(int width, int height,
                                                     int channels) noexcept;

private:
  std::span<const std::uint8_t> original_; /**< Original image data view. */
  std::span<const std::uint8_t>
      watermarked_; /**< Watermarked image data view. */
};

} // namespace metric

#endif
