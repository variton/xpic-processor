
#ifndef JPEG_DECODER_H
#define JPEG_DECODER_H

#include <error_info.h>
#include <errty.h>
#include <jerr.h>
#include <jpeg_decompressor.h>
#include <ncnm.h>
#include <tl/expected.hpp>

#include <cstdint>
#include <memory>
#include <span>

namespace img {

template <typename T> using Default = core::NCNM<T>;

enum class JpegDecoderError { JpegDecodeError };

using JpegDecoderErrorInfo = err::ErrorInfo<JpegDecoderError>;

class JpegDecoder : public Default<JpegDecoder> {

public:
  explicit JpegDecoder(int width, int height, int channels) noexcept;
  ~JpegDecoder();
  tl::expected<void, JpegDecoderErrorInfo>
  decode(JpegDecompressor &decompressor) noexcept;
  std::span<const std::uint8_t> pixels() const noexcept;

private:
  std::size_t size_pixels_buffer_;
  std::unique_ptr<uint8_t[]> pixels_;
  JpegError err_;
};

} // namespace img

#endif
