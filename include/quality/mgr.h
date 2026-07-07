
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

/**
 * @brief Default CRTP base alias for metric managers.
 */
template <typename T> using Default = core::NCNM<T>;

/**
 * @brief Errors that may occur while computing the metrics.
 */
enum class MgrError { PSNRError, SSIMError };

ERR_DEFINE_ERROR_INFO(MgrError, MgrErrorInfo);

class Mgr : public Default<Mgr> {
public:
  explicit Mgr(std::span<const std::uint8_t> original,
               std::span<const std::uint8_t> watermarked, std::size_t width,
               std::size_t height) noexcept;

  ~Mgr();

  tl::expected<double, MgrErrorInfo> computePNSR(int channels) noexcept;
  tl::expected<double, MgrErrorInfo> computeSSIM() noexcept;

private:
  metric::PSNRMgr psnr_mgr_{{}, {}};
  metric::SSIMMgr ssim_mgr_{{}, {}};
  std::size_t width_;
  std::size_t height_;
};

} // namespace quality

#endif // MGR_H
