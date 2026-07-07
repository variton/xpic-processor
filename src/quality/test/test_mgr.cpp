#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>

#include <cstdint>
#include <string>

#include <env_mgr.h>
#include <filehandler.h>
#include <fmt/core.h>
#include <inputimg.h>
#include <jpeg_decoder.h>
#include <jpeg_decompressor.h>
#include <mgr.h>

TEST_CASE("Mgr computePnsr OK") {
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
  // mgr ===============================================================
  quality::Mgr mgr{original.pixels(), watermarked.pixels(),
                   static_cast<std::size_t>(inputimg.width),
                   static_cast<std::size_t>(inputimg.height)};
  auto ret = mgr.computePNSR(inputimg.components);
  CHECK(ret.has_value());
  volatile double pnsr = ret.value();
  CHECK(ret.value() > 0);
}

// todo fix test coverage
// TEST_CASE("Blender blend fails") {
// }
