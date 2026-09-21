#ifndef PIXEL_MGR_H
#define PIXEL_MGR_H

#include <filehandler.h>
#include <inputimg.h>
#include <jpeg_decoder.h>
#include <memory>
#include <ncnm.h>
#include <span>
#include <string>
#include <string_view>

namespace img {

template <typename T>
using Default = core::NCNM<T>;

/** @brief Errors encountered while loading or decoding an image. */
enum class PixelMgrError {
  FileOpenInitError,     ///< Failed to open the image file.
  DecompressorInitError, ///< Failed to initialize the JPEG decompressor.
  DecompressError,       ///< Failed to decode the JPEG image.
};

ERR_DEFINE_ERROR_INFO(PixelMgrError, PixelMgrErrorInfo);

/** @brief Loads and decodes pixel data from a JPEG file. */
class PixelMgr : public Default<PixelMgr> {
public:
  /**
   * @brief Constructs a pixel manager.
   * @param filepath Path to the JPEG file.
   */
  explicit PixelMgr(std::string_view filepath) noexcept;

  /**
   * @brief Opens and decodes the JPEG file.
   * @return Success or details of the encountered error.
   */
  [[nodiscard]] tl::expected<void, PixelMgrErrorInfo> init() noexcept;

  /**
   * @brief Returns the decoded pixel data.
   * @return A pixel view, empty before successful initialization.
   * @note The view is invalidated by init() or object destruction.
   */
  [[nodiscard]] std::span<uint8_t> pixels() const noexcept;

  /**
   * @brief Returns the decoded image description.
   * @return The image description.
   */
  [[nodiscard]] InputImg img() const noexcept;

private:
  InputImg input_img;
  std::string filepath_;
  std::unique_ptr<JpegDecoder> decoder_;
};

} // namespace img

#endif // PIXEL_MGR_H
