
#include <jpeg_decoder.h>

namespace img {

JpegDecoder::JpegDecoder(int width, int height, int channels) noexcept
    : size_pixels_buffer_(static_cast<std::size_t>(width) *
                          static_cast<std::size_t>(height) *
                          static_cast<std::size_t>(channels)),
      pixels_{std::make_unique<uint8_t[]>(size_pixels_buffer_)} {}

JpegDecoder::~JpegDecoder() {}

tl::expected<void, JpegDecoderErrorInfo>
JpegDecoder::decode(JpegDecompressor &decompressor) noexcept {

  auto &cinfo = decompressor.cinfo();

  if (setjmp(err_.setjmp_buf))
    return propagate_err(err_, JpegDecoderError::JpegDecodeError,
                         "JPEG decode error");

  while (cinfo.output_scanline < cinfo.output_height) {
    JSAMPROW row = pixels_.get() + cinfo.output_scanline * cinfo.output_width *
                                       cinfo.output_components;

    jpeg_read_scanlines(&cinfo, &row, 1);
  }

  return {};
}

std::span<const std::uint8_t> JpegDecoder::pixels() const noexcept {
  return {pixels_.get(), size_pixels_buffer_};
}

} // namespace img
