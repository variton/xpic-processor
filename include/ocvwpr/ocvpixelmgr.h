/// @file
/// @brief OpenCV image loading and pixel access.

#ifndef OCV_PIXEL_MGR_H
#define OCV_PIXEL_MGR_H

#include <ncnm.h>
#include <span>
#include <string>
#include <opencv2/opencv2.hpp>
#include <tuple>

namespace ocvwpr{

/// @brief Base type for non-copyable, non-movable objects.
template <typename T> using Default = core::NCNM<T>;


/// @brief Errors reported while loading an image.
enum class OcvPixelMgrError {
  LoadImageError,     ///< Failed to load buffer before accessing pixels.
};

/// @brief Loads an image and provides access to its pixel data.
class OcvPixelMgr:public Default<OcvPixelMgr>{
public:

  /// @brief Creates a pixel manager for an image file.
  /// @param filepath Path to the image file.
  explicit OcvPixelMgr(std::string filepath) noexcept;

  /// @brief Loads the image from the configured path.
  /// @return An empty success value or details of the loading error.
  [[nodiscard]] tl::expected<void, OcvPixelMgrErrorInfo> init() noexcept;
  
  /// @return A mutable, non-owning view of the image's pixel bytes.
  [[nodiscard]] std::span<uint8_t> pixels() const noexcept;

  /// @brief Reads the color channels of a pixel.
  /// @param x Zero-based column index.
  /// @param y Zero-based row index.
  /// @return The blue, green, and red channel values, in that order.
  [[nodiscard]] std::tuple<uint8_t,uint8_t,uint8_t> getChannels(uint8_t x,uint8_t y) noexcept;

  /// @return The size of the pixel data in bytes.
  [[nodiscard]] const std::size_t std::byteCount() const noexcept;

  /// @return The image as an OpenCV matrix.
  [[nodiscard]] cv::Mat img() const noexcept;

private:
  std::string filepath_;
  cv::Mat image_;
};

} // namespace ocvwpr

#endif
