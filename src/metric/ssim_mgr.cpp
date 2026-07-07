
#include <cmath>
#include <ssim_mgr.h>

namespace metric {

SSIMMgr::SSIMMgr(std::span<const std::uint8_t> original,
                 std::span<const std::uint8_t> watermarked, std::size_t window,
                 double k1, double k2, double l) noexcept
    : original_{original}, watermarked_{watermarked}, window_{window}, k1_{k1},
      k2_{k2}, l_{l} {}

SSIMMgr::~SSIMMgr() = default;

tl::expected<double, SSIMMgrErrorInfo>
SSIMMgr::computeSSIM(int width, int height) noexcept {

  int radius = window_ / 2;

  const double C1 = (k1_ * l_) * (k1_ * l_);
  const double C2 = (k2_ * l_) * (k2_ * l_);

  double totalSSIM = 0.0;
  int windows = 0;

  for (int cy = radius; cy < height - radius; ++cy) {
    for (int cx = radius; cx < width - radius; ++cx) {
      double mean1 = 0.0;
      double mean2 = 0.0;

      //---------------------------------
      // Means
      //---------------------------------

      for (int y = -radius; y <= radius; ++y) {
        for (int x = -radius; x <= radius; ++x) {
          const int idx = (cy + y) * width + (cx + x);

          mean1 += original_[idx];
          mean2 += watermarked_[idx];
        }
      }

      mean1 /= (window_ * window_);
      mean2 /= (window_ * window_);

      //---------------------------------
      // Variance & Covariance
      //---------------------------------

      double var1 = 0.0;
      double var2 = 0.0;
      double cov = 0.0;

      for (int y = -radius; y <= radius; ++y) {
        for (int x = -radius; x <= radius; ++x) {
          const int idx = (cy + y) * width + (cx + x);

          const double a = original_[idx] - mean1;
          const double b = watermarked_[idx] - mean2;

          var1 += a * a;
          var2 += b * b;
          cov += a * b;
        }
      }

      var1 /= (window_ * window_ - 1);
      var2 /= (window_ * window_ - 1);
      cov /= (window_ * window_ - 1);

      //---------------------------------
      // SSIM
      //---------------------------------

      const double numerator = (2.0 * mean1 * mean2 + C1) * (2.0 * cov + C2);

      const double denominator =
          (mean1 * mean1 + mean2 * mean2 + C1) * (var1 + var2 + C2);

      totalSSIM += numerator / denominator;
      ++windows;
    }
  }
  // temp
  if (windows == 0) {
    return err::unexpected(SSIMMgrError::LowValError,
                           "Not any embedded watermarked");
  }

  return totalSSIM / windows;
}

} // namespace metric
