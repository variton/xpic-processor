
#include <pixel_mgr.h>
#include <inputimg.h>
#include <jpeg_decoder.h>

namespace img {

PixelMgr::PixelMgr(std::string_view filepath) noexcept: filepath_(filepath) {
}

tl::expected<void, PixelMgrErrorInfo> PixelMgr::init() noexcept { 
  auto ret_open = fio::FileHandler::open(filepath_, "rb");
  if (!ret_open)  return err::unexpected(PixelMgrError::FileOpenInitError,
                                         ret_open.error().message);

  filehandler_ = std::move(result.value());

  auto ret_decompressor_init = decompressor_.init(filehandler_.get());
  if (!ret_decompressor_init) return err::unexpected(PixelMgrError::DecompressorInitError,
                                                     ret_decompressor_init.error().message);

  auto ret_decompress = decompressor_.decompress();
  if (!ret_decompressor) return err::unexpected(PixelMgrError::DecompressorInitError,
                                                     ret_decompress.error().message);
  InputImg inputimg{decompressor_.cinfo()};
  JpegDecoder decoder{inputimg.width, inputimg.height, inputimg.components};
  decoder.decode(decompressor_);

  return {}; 
}

std::span<uint8_t> PixelMgr::pixels() const noexcept { 
  return decompressor_.pixels();
}


} // namespace img
