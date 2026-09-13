
#include <array>
#include <channel_mgr.h>
#include <mean_ssim_mgr.h>
#include <numeric>
#include <ssim_mgr.h>
#include <thread>

namespace metric {

MeanSSIMMgr::MeanSSIMMgr(std::span<const std::uint8_t> original,
                         std::span<const std::uint8_t> watermarked) noexcept
    : original_{original}, watermarked_{watermarked} {}

MeanSSIMMgr::~MeanSSIMMgr() = default;
// todo check how the errors are propagated
tl::expected<double, MeanSSIMMgrErrorInfo>
MeanSSIMMgr::computeMeanSSIM(int width, int height, int components) noexcept {

  img::ChannelMgr channel_mgr{width, height, components};
  auto original_res = channel_mgr.splitRGBChannels(original_);
  auto watermarked_res = channel_mgr.splitRGBChannels(watermarked_);

  std::array<double, 3> channel_ssim{};

  for (std::size_t channel = 0; channel < 3; ++channel) {
    SSIMMgr ssim_mgr{original_res.value()[channel],
                     watermarked_res.value()[channel]};

    auto result = ssim_mgr.computeSSIM(width, height);

    channel_ssim[channel] = result.value();
  }

  return std::accumulate(channel_ssim.begin(), channel_ssim.end(), 0.0) / 3.0;
}

tl::expected<double, MeanSSIMMgrErrorInfo>
MeanSSIMMgr::MtComputeMeanSSIM(int width, int height, int components) noexcept {
  img::ChannelMgr channel_mgr{width, height, components};

  auto original_res = channel_mgr.splitRGBChannels(original_);
  auto watermarked_res = channel_mgr.splitRGBChannels(watermarked_);

  auto original_component = original_res.value();
  auto watermarked_component = watermarked_res.value();

  std::array<tl::expected<double, MeanSSIMMgrErrorInfo>, 3> channel_results{};

  {
    std::jthread t1{[&] {
      SSIMMgr ssim_mgr{original_component[0], watermarked_component[0]};
      channel_results[0] = ssim_mgr.computeSSIM(width, height).value();
    }};

    std::jthread t2{[&] {
      SSIMMgr ssim_mgr{original_component[1], watermarked_component[1]};
      channel_results[1] = ssim_mgr.computeSSIM(width, height).value();
    }};

    std::jthread t3{[&] {
      SSIMMgr ssim_mgr{original_component[2], watermarked_component[2]};
      channel_results[2] = ssim_mgr.computeSSIM(width, height).value();
    }};
  } // all three joined here before we touch channel_results

  for (const auto &result : channel_results) {
    if (!result.has_value()) {
      return err::propagate(result, MeanSSIMMgrError::MeanSSIMChannelError);
    }
  }

  double sum = channel_results[0].value() + channel_results[1].value() +
               channel_results[2].value();
  return sum / 3.0;
}

} // namespace metric
