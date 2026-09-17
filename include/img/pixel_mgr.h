
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

template <typename T> using Default = core::NCNM<T>;

/**
 * @brief Errors that may occur while initializing or decoding an image.
 */
enum class PixelMgrError {
  /** @brief The image file could not be opened. */
  FileOpenInitError,

  /** @brief The JPEG decompressor could not be initialized. */
  DecompressorInitError,

  /** @brief JPEG decompression failed. */
  DecompressError,
};

ERR_DEFINE_ERROR_INFO(PixelMgrError, PixelMgrErrorInfo);

/**
 * @brief Manages loading and decoding pixel data from a JPEG file.
 */
class PixelMgr : public Default<PixelMgr> {
public:
  /**
   * @brief Constructs a pixel manager for the specified file.
   * @param filepath Path to the JPEG file.
   */
  explicit PixelMgr(std::string_view filepath) noexcept;

  /**
   * @brief Opens and decodes the configured JPEG file.
   * @return Success, or error information describing the failure.
   */
  [[nodiscard]] tl::expected<void, PixelMgrErrorInfo> init() noexcept;

  /**
   * @brief Returns the decoded pixel data.
   * @return A view of the decoded pixels. Empty before successful
   *         initialization.
   *
   * @note The returned view remains valid until the next call to init() or
   *       until this object is destroyed.
   */
  [[nodiscard]] std::span<uint8_t> pixels() const noexcept;

  [[nodiscard]] InputImg img() const noexcept;

private:
  InputImg input_img;
  std::string filepath_;
  std::unique_ptr<JpegDecoder> decoder_;
};

} // namespace img

#endif // PIXEL_MGR_H
