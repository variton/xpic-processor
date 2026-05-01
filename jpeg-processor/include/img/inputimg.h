
#ifndef INPUT_IMG_H
#define INPUT_IMG_H

#include <jpeg_decompressor.h>

namespace img {

// P O D
struct InputImg {
  int width;
  int height;
  int components;
  int row_stride;

  InputImg() noexcept : width{0}, height{0}, components{0}, row_stride{0} {}

  explicit InputImg(const jpeg_decompress_struct &cinfo) noexcept
      : width{static_cast<int>(cinfo.output_width)},
        height{static_cast<int>(cinfo.output_height)},
        components{cinfo.output_components}, row_stride{width * components} {}
};

} // namespace img

#endif // INPUT_IMG_H
