#ifndef MGR_H
#define MGR_H

#include <cstdint>
#include <err_utils.h>
#include <ncnm.h>
#include <pnsr_mgr.h>
#include <span>
#include <ssim_mgr.h>
#include <tl/expected.hpp>

namespace quality {

/** Default CRTP base for metric managers. */
template <typename T> using Default = core::NCNM<T>;

/** Errors produced during metric computation. */
enum class MgrError {
  PSNRError, /**< PSNR computation failed. */
  SSIMError  /**< SSIM computation failed. */
};

ERR_DEFINE_ERROR_INFO(MgrError, MgrErrorInfo);

/** Computes image-quality metrics for an original and watermarked image. */
class Mgr : public Default<Mgr> {
public:
  /**
   * @brief Constructs a metric manager.
   * @param original Original image data.
   * @param watermarked Watermarked image data.
   * @param width Image width in pixels.
   * @param height Image height in pixels.
   */
  explicit Mgr(std::span<const std::uint8_t> original,
               std::span<const std::uint8_t> watermarked, std::size_t width,
               std::size_t height) noexcept;

  ~Mgr();

  /**
   * @brief Computes the PSNR.
   * @param channels Number of image channels.
   * @return The PSNR value, or error information on failure.
   */
  [[nodiscard]] tl::expected<double, MgrErrorInfo>
  computePNSR(int channels) noexcept;

  /**
   * @brief Computes the SSIM.
   * @return The SSIM value, or error information on failure.
   */
  [[nodiscard]] tl::expected<double, MgrErrorInfo> computeSSIM() noexcept;

private:
  metric::PSNRMgr psnr_mgr_{{}, {}};
  metric::SSIMMgr ssim_mgr_{{}, {}};
  std::size_t width_;
  std::size_t height_;
};

} // namespace quality

#endif // MGR_H
