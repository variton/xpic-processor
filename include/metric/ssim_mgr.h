
#ifndef SSIM_MGR_H
#define SSIM_MGR_H

#include <cstdint>
#include <ncnm.h>
#include <span>

#include <error_info.h>
#include <errty.h>
#include <tl/expected.hpp>


namespace metric{

template <typename T> using Default = core::NCNM<T>;

enum class SSIMMgrError {
  LowValError,
};

using SSIMMgrErrorInfo = err::ErrorInfo<SSIMMgrError>;

static_assert(topology::ErrorInfoTy<SSIMMgrErrorInfo>);

class SSIMMgr:public Default<SSIMMgr> {

public:

  explicit SSIMMgr(std::span<const std::uint8_t> original,
                   std::span<const std::uint8_t> watermarked,
                   std::size_t window=11,
                   double k1=0.01, 
                   double k2=0.03, 
                   double l=255.0) noexcept;

  ~SSIMMgr();

  tl::expected<double, SSIMMgrErrorInfo> computeSSIM(int width, int height) noexcept;

private:
  std::span<const std::uint8_t> original_;    /**< Original image data view. */
  std::span<const std::uint8_t> watermarked_; /**< Watermarked image data view. */
  std::size_t window_;
  double k1_;
  double k2_;
  double l_;

};

}

#endif 
