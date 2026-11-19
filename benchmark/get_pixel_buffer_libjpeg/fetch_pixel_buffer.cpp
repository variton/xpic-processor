
#include <fmt/core.h>
#include <runtime.h>

#include <pixel_mgr.h>
#include <mean_ssim_mgr.h>

//quality check to gather picture metric
//argv[1] input path original
//argv[2] input path watermarked
MILLI_CLOCK_RUNTIME_MAIN {
  clock();
  img::PixelMgr pixel_mgr_o{argv[1]};
  auto ret_init_pxmgr_o = pixel_mgr_o.init();
  clock();
  fmt::println("time elasped: {} ms", clock.elapsed());
  return 0;
}
