
#ifndef PIXEL_MGR_H
#define PIXEL_MGR_H

#include <filehandler.h>
#include <jpeg_decoder.h>
#include <memory>
#include <ncnm.h>
#include <span>
#include <string>
#include <string_view>

namespace img {

template <typename T> using Default = core::NCNM<T>;

enum class PixelMgrError {
  /** @brief The file could not be opened. */
  FileOpenInitError,
  DecompressorInitError,
  DecompressError,
};

ERR_DEFINE_ERROR_INFO(PixelMgrError, PixelMgrErrorInfo);

class PixelMgr : public Default<PixelMgr> {
public:
  explicit PixelMgr(std::string_view filepath) noexcept;
  tl::expected<void, PixelMgrErrorInfo> init() noexcept;
  // Empty before successful initialization; valid until the next init() or
  // destruction.
  std::span<uint8_t> pixels() const noexcept;

private:
  std::string filepath_;
  std::unique_ptr<JpegDecoder> decoder_;
};

} // namespace img
#endif // PIXEL_MGR_H
