#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>

#include <string>

#include <env_mgr.h>
#include <filehandler.h>
#include <fmt/core.h>
#include <inputimg.h>
#include <jpeg_decoder.h>
#include <jpeg_decompressor.h>
#include <pnsr_mgr.h>

TEST_CASE("ComputePnsr OK") {
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
  metric::PSNRMgr pnsr_mgr{original.pixels(), watermarked.pixels()};
  auto ret_pnsr = pnsr_mgr.computePNSR(inputimg.width, inputimg.height,
                                       inputimg.components);
  CHECK(ret_pnsr.has_value());
  volatile double pnsr = ret_pnsr.value();
  CHECK(ret_pnsr.value() > 0);
}

// todo fix test coverage
// TEST_CASE("Blender blend fails") {
// }
