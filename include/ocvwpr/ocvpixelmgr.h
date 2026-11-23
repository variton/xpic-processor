
#ifndef OCV_PIXEL_MGR_H
#define OCV_PIXEL_MGR_H

#include <ncnm.h>
#include <span>
#include <string>
#include <opencv2/opencv2.hpp>
#include <tuple>

namespace ocvwpr{

template <typename T> using Default = core::NCNM<T>;


enum class OcvPixelMgrError {
  LoadImageError,     ///< Failed to load buffer before accessing pixels.
};

class OcvPixelMgr:public Default<OcvPixelMgr>{
public:

  explicit OcvPixelMgr(std::string filepath) noexcept;

  [[nodiscard]] tl::expected<void, OcvPixelMgrErrorInfo> init() noexcept;
  
  [[nodiscard]] std::span<uint8_t> pixels() const noexcept;

  //returns a tuple that contains BGR channels
  [[nodiscard]] std::tuple<uint8_t,uint8_t,uint8_t> getChannels(uint8_t x,uint8_t y) noexcept;

  [[nodiscard]] const std::size_t std::byteCount() const noexcept;

  [[nodiscard]] cv::Mat img() const noexcept;

private:
  std::string filepath_;
  cv::Mat image_;
};

} // namespace ocvwpr

#endif
