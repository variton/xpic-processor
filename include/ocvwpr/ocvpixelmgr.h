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
/// @tparam T Derived type.
template <typename T> using Default = core::NCNM<T>;


/// @brief Errors reported while loading an image.
enum class OcvPixelMgrError {
  LoadImageError,     ///< The image could not be loaded from the configured path.
};

/// @brief Loads an image and provides access to its pixel data.
/// @details Call init() and check its result before accessing the image.
class OcvPixelMgr:public Default<OcvPixelMgr>{
public:

  /// @brief Stores the path for a subsequent call to init().
  /// @param filepath Path to the image file.
  explicit OcvPixelMgr(std::string filepath) noexcept;

  /// @brief Loads the image from the configured path.
  /// @return Success if the image was loaded, or error details with
  ///         OcvPixelMgrError::LoadImageError on failure.
  [[nodiscard]] tl::expected<void, OcvPixelMgrErrorInfo> init() noexcept;
  
  /// @brief Provides direct access to the loaded image's pixel bytes.
  /// @pre init() has succeeded.
  /// @return A mutable, non-owning span. Writes through it modify the image.
  /// @warning The span does not keep the pixel storage alive. Obtain a new span
  ///          after reloading the image.
  [[nodiscard]] std::span<uint8_t> pixels() const noexcept;

  /// @brief Reads the color channels of a pixel.
  /// @pre init() has succeeded and the coordinates are within the image bounds.
  /// @param x Zero-based column index, measured from the left.
  /// @param y Zero-based row index, measured from the top.
  /// @return The blue, green, and red channel values, in that order.
  [[nodiscard]] std::tuple<uint8_t,uint8_t,uint8_t> getChannels(uint8_t x,uint8_t y) noexcept;

  /// @brief Reports the size of the loaded pixel data.
  /// @pre init() has succeeded.
  /// @return The number of bytes in the pixel buffer.
  [[nodiscard]] const std::size_t std::byteCount() const noexcept;

  /// @brief Provides the loaded image as an OpenCV matrix.
  /// @pre init() has succeeded.
  /// @return A shallow copy of the matrix that shares the image's pixel data.
  ///         Use cv::Mat::clone() if you need an independent copy.
  [[nodiscard]] cv::Mat img() const noexcept;

private:
  std::string filepath_;
  cv::Mat image_;
};

} // namespace ocvwpr

#endif
