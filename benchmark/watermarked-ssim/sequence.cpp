
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

  auto ret_ssim = mgr.computeSSIM();
  if (!ret_ssim) fmt::println("{}",ret_ssim.error().message);
  clock();
  fmt::println("time elasped: {} ms", clock.elapsed());
  return 0;
}

//MILLI_CLOCK_RUNTIME_MAIN {
//  clock();
//  //fmt::println("{}",argv[1]);
//  //quality check to gather picture metric 
//  //argv[1] input path
//  //argv[2] output path pnsr ssim
//  img::PixelMgr pixel_mgr_o{argv[1]};
//  auto ret_init_pxmgr_o = pixel_mgr_o.init();
//  //if (!ret_init_pxmgr_o) fmt::println("original pixel_mgr init failed");
  
//  img::PixelMgr pixel_mgr_wd{argv[2]};
//  auto ret_init_pxmgr_wd = pixel_mgr_wd.init();
//  //if (!ret_init_pxmgr_wd) fmt::println("watermarked pixel_mgr init failed");

//  //clock();
//  //fmt::println("time elasped: {} ms", clock.elapsed());
//  //clock();

//  quality::Mgr mgr{pixel_mgr_o.pixels(),
//                   pixel_mgr_wd.pixels(),
//                   static_cast<size_t>(pixel_mgr_o.img().width),
//                   static_cast<size_t>(pixel_mgr_o.img().height)};
 
//  auto ret_pnsr = mgr.computePNSR(pixel_mgr_o.img().components);
//  //auto ret_ssim = mgr.computeSSIM();
//  if (!ret_pnsr) fmt::println("{}",ret_pnsr.error().message);
//  //if (!ret_ssim) fmt::println("ssim computation failed");
//  //volatile double pnsr = ret_pnsr.value();
//  //volatile double ssim = ret_ssim.value();
//  //fmt::println("pnsr: {}", pnsr);
//  //fmt::println("ssim: {}", ssim);
//  clock();
//  fmt::println("time elasped: {} ms", clock.elapsed());
//  return 0;
//}
