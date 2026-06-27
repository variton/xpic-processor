
#include <pnsr_mgr.h>
#include <limits>
#include <cmath>

namespace metric {

const double BASE10{10.0};
const double MAX_PIXEL_VALUE{255.0};

PSNRMgr::PSNRMgr(std::span<const std::uint8_t> original, 
                 std::span<const std::uint8_t> watermarked) noexcept : 
                 original_{original}, 
                 watermarked_{watermarked} {}

PSNRMgr::~PSNRMgr() = default;

tl::expected<double,PSNRMgrErrorInfo> PSNRMgr::computePNSR(int width,int height,int channels) noexcept {
 
  const size_t N = static_cast<size_t>(width) * static_cast<size_t>(height) * static_cast<size_t>(channels);

  double sse = 0.0;

  for (size_t i = 0; i < N; ++i)
  {
    const int diff = static_cast<int>(original_[i]) - static_cast<int>(watermarked_[i]);
    sse += static_cast<double>(diff * diff);
  }

  if (sse == 0.0){
    return err::unexpected(PSNRMgrError::SSENullError,"Not any embedded watermarked");
  }
  const double mse = sse / N;

  return BASE10 * std::log10((MAX_PIXEL_VALUE * MAX_PIXEL_VALUE) / mse);
}

} // namespace metric 
