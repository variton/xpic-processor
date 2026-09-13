
#ifndef CHANNEL_MGR_H
#define CHANNEL_MGR_H

#include <err_utils.h>
#include <ncnm.h>
#include <tl/expected.hpp>

#include <array>
#include <cstdint>
#include <span>
#include <vector>

namespace img {

/**
 * @brief Default non-copyable, non-movable base type.
 *
 * @tparam T Derived type.
 */
template <typename T> using Default = core::NCNM<T>;

/** @brief Separate red, green, and blue byte channels, in that order. */
using Channels = std::array<std::vector<std::uint8_t>, 3>;

/** @brief Errors that may occur during channel processing. */
enum class ChannelError {
  ChannelImgDimError,  /**< Invalid image dimensions. */
  ChannelStructError,  /**< Invalid channel configuration. */
  ChannelSplitRGBError /**< RGB channel separation failed. */
};

ERR_DEFINE_ERROR_INFO(ChannelError, ChannelErrorInfo);

/** @brief Splits interleaved image pixels into RGB channels. */
class ChannelMgr : public core::NCNM<ChannelMgr> {
public:
  /**
   * @brief Constructs a channel manager.
   *
   * @param width Image width in pixels.
   * @param height Image height in pixels.
   * @param components Number of components per pixel.
   */
  explicit ChannelMgr(int width, int height, int components) noexcept;

  /** @brief Destroys the channel manager. */
  ~ChannelMgr();

  /**
   * @brief Splits interleaved pixel data into red, green, and blue channels.
   *
   * @param pixels Input pixel data.
   * @return RGB channels on success, or error information on failure.
   */
  tl::expected<Channels, ChannelErrorInfo>
  splitRGBChannels(std::span<const std::uint8_t> pixels) noexcept;

private:
  /** @brief Validates the configured dimensions and channel structure. */
  tl::expected<void, ChannelErrorInfo> has_valid_channels() noexcept;

  int width_;      /**< Image width in pixels. */
  int height_;     /**< Image height in pixels. */
  int components_; /**< Number of components per pixel. */
};

} // namespace img

#endif
