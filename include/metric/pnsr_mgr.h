
#ifndef PSNR_MGR_H
#define PSNR_MGR_H

#include <cstdint>
#include <ncnm.h>
#include <span>

#include <error_info.h>
#include <errty.h>
#include <tl/expected.hpp>

namespace metric {

template <typename T> using Default = core::NCNM<T>;

enum class PSNRMgrError {
  SSENullError,
};

using PSNRMgrErrorInfo = err::ErrorInfo<PSNRMgrError>;

static_assert(topology::ErrorInfoTy<PSNRMgrErrorInfo>);

class PSNRMgr : public Default<PSNRMgr> {

public:
  explicit PSNRMgr(std::span<const std::uint8_t> original,
                   std::span<const std::uint8_t> watermarked) noexcept;
  ~PSNRMgr();

  tl::expected<double, PSNRMgrErrorInfo> computePNSR(int width, int height,
                                                     int channels) noexcept;

private:
  std::span<const std::uint8_t> original_;
  std::span<const std::uint8_t> watermarked_;
};

} // namespace metric

#endif
