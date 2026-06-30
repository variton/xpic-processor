#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>

#include <string>

#include <env_mgr.h>
#include <filehandler.h>
#include <fmt/core.h>
#include <inputimg.h>
#include <jpeg_decoder.h>
#include <jpeg_decompressor.h>
#include <ssim_mgr.h>

TEST_CASE("ComputeSSIM OK") {
  // auto &local = platform::EnvMgr::get_instance();
  // auto rc = local.get_env_var("RC");
  // temporary patch
  std::string rc{"/home/cxx-core/rc"};
  using namespace img;
  std::string input{rc + "/ur.jpg"};
  auto input_file = fio::FileHandler::open(input.data(), "rb");
  CHECK(input_file.has_value());
  // Jpeg decompressor creation
  // ==================================================
  JpegDecompressor decompressor{};
  auto ret_init_decompressor = decompressor.init(input_file.value().get());
  CHECK(ret_init_decompressor.has_value());
  auto ret_decompress = decompressor.decompress();
  CHECK(ret_decompress.has_value());
  // image object creation ==================================================
  InputImg inputimg{decompressor.cinfo()};
  // jpeg decoder ===========================================================
  /*the decoding is a bottleneck 2x time*/
  JpegDecoder original{inputimg.width, inputimg.height, inputimg.components};
  original.decode(decompressor);
  JpegDecoder watermarked{inputimg.width, inputimg.height, inputimg.components};
  watermarked.decode(decompressor);
  // pnsr mgr ===============================================================
  metric::SSIMMgr ssim_mgr{original.pixels(), watermarked.pixels()};
  auto ret_ssim = ssim_mgr.computeSSIM(inputimg.width, inputimg.height);
  CHECK(ret_ssim.has_value());
  volatile double ssim = ret_ssim.value();
  CHECK(ret_ssim.value() <= 1);
}

// todo fix test coverage
// TEST_CASE("Blender blend fails") {
// }
