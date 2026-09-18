
#include <mgr.h>
#include <fmt/core.h>
#include <runtime.h>

#include <pixel_mgr.h>
#include <mgr.h>

//quality check to gather picture metric
//argv[1] input path original
//argv[2] input path watermarked
MILLI_CLOCK_RUNTIME_MAIN {
  clock();
  img::PixelMgr pixel_mgr_o{argv[1]};
  auto ret_init_pxmgr_o = pixel_mgr_o.init();

  img::PixelMgr pixel_mgr_wd{argv[2]};
  auto ret_init_pxmgr_wd = pixel_mgr_wd.init();

  quality::Mgr mgr{pixel_mgr_o.pixels(),
                   pixel_mgr_wd.pixels(),
                   static_cast<size_t>(pixel_mgr_o.img().width),
                   static_cast<size_t>(pixel_mgr_o.img().height)};

  auto ret_pnsr = mgr.computePNSR(pixel_mgr_o.img().components);
  if (!ret_pnsr) fmt::println("{}",ret_pnsr.error().message);
  clock();
  fmt::println("time elasped: {} ms", clock.elapsed());
  return 0;
}
