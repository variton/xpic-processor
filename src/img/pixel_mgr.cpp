#include <pixel_mgr.h>

namespace img {

PixelMgr::PixelMgr(std::string_view filepath) noexcept : filepath_(filepath) {}

tl::expected<void, PixelMgrErrorInfo> PixelMgr::init() noexcept {
  decoder_.reset();

  auto file = fio::FileHandler::open(filepath_, "rb");
  if (!file)
    return err::unexpected(PixelMgrError::FileOpenInitError,
                           file.error().message);

  JpegDecompressor decompressor;
  auto initialized = decompressor.init(file->get());
  if (!initialized)
    return err::unexpected(PixelMgrError::DecompressorInitError,
                           initialized.error().message);

  auto started = decompressor.decompress();
  if (!started)
    return err::unexpected(PixelMgrError::DecompressError,
                           started.error().message);

  const auto &cinfo = decompressor.cinfo();
  auto decoder = std::make_unique<JpegDecoder>(
      cinfo.output_width, cinfo.output_height, cinfo.output_components);

  auto decoded = decoder->decode(decompressor);
  if (!decoded)
    return err::unexpected(PixelMgrError::DecompressError,
                           decoded.error().message);

  auto finished = decompressor.finish_decompress();
  if (!finished)
    return err::unexpected(PixelMgrError::DecompressError,
                           finished.error().message);

  decoder_ = std::move(decoder);
  return {};
}

std::span<uint8_t> PixelMgr::pixels() const noexcept {
  return decoder_ ? decoder_->pixels() : std::span<uint8_t>{};
}

} // namespace img
