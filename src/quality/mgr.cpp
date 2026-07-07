
#include <mgr.h>

namespace quality {

Mgr::Mgr(std::span<const std::uint8_t> original,
         std::span<const std::uint8_t> watermarked, std::size_t width,
         std::size_t height) noexcept
    : psnr_mgr_{original, watermarked}, ssim_mgr_{original, watermarked},
      width_{width}, height_{height} {}

Mgr::~Mgr() {}

tl::expected<double, MgrErrorInfo> Mgr::computePNSR(int channels) noexcept {
  auto ret = psnr_mgr_.computePNSR(width_, height_, channels);
  if (!ret.has_value()) {
    return err::unexpected(MgrError::PSNRError, ret.error().message);
  }
  return ret.value();
}

tl::expected<double, MgrErrorInfo> Mgr::computeSSIM() noexcept {
  auto ret = ssim_mgr_.computeSSIM(width_, height_);
  if (!ret.has_value()) {
    return err::unexpected(MgrError::SSIMError, ret.error().message);
  }
  return ret.value();
}

} // namespace quality
