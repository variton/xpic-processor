#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>

#include <string>

#include <filehandler.h>
#include <inputimg.h>
#include <pnsr_mgr.h>
#include <env_mgr.h>

// #include <filehandler.h>
// #include <inputimg.h>
// #include <jpeg_compressor.h>
#include <jpeg_decompressor.h>

TEST_CASE("ComputePnsr OK") {
  auto &local = platform::EnvMgr::get_instance();
  auto rc = local.get_env_var("RC");
  using namespace img;
  std::string input{rc.value()+"/ur.jpg"};
  auto input_file = fio::FileHandler::open(input.data(), "rb");
  CHECK(input_file.has_value());

  JpegDecompressor decompressor{};
  auto ret_init_decompressor = decompressor.init(input_file.value().get());
  CHECK(ret_init_decompressor.has_value());
  auto ret_decompress = decompressor.decompress();
  CHECK(ret_decompress.has_value());
  // image object creation ==================================================
  InputImg inputimg{decompressor.cinfo()};

  // pnsr mgr ===============================================================
  //auto pnsr_mgr = metric::PSNRMgr{};
  //auto pnsr = pnsr_mgr.computePNSR(inputimg.width, inputimg.height, inputimg.components);
}

// todo fix test coverage
// TEST_CASE("Blender blend fails") {
// }
