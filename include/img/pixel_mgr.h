
#ifndef PIXEL_MGR_H
#define PIXEL_MGR_H

#include <filehandler.h>
#include <jpeg_decompressor.h>
#include <ncnm.h>
#include <span>
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
  std::span<uint8_t> pixels() const noexcept;

private:
  std::string_view filepath_;
  fio::FileHandler filehdr_;
  JpegDecompressor decompressor_;
};

} // namespace img
#endif // PIXEL_MGR_H
