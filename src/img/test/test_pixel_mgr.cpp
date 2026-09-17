#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <algorithm>
#include <cstdlib>
#include <pixel_mgr.h>

TEST_CASE("PixelMgr retains the decoded buffer") {
  std::string rc{"/home/cxx-core/rc"};
  using namespace img;
  std::string input{rc + "/ur.jpg"};
  img::PixelMgr manager{std::string(input)};
  CHECK(manager.pixels().empty());
  REQUIRE(manager.init().has_value());

  auto file = fio::FileHandler::open(input, "rb");
  REQUIRE(file.has_value());
  img::JpegDecompressor decompressor;
  REQUIRE(decompressor.init(file->get()).has_value());
  REQUIRE(decompressor.decompress().has_value());
  const auto &info = decompressor.cinfo();
  img::JpegDecoder reference{static_cast<int>(info.output_width),
                             static_cast<int>(info.output_height),
                             info.output_components};
  REQUIRE(reference.decode(decompressor).has_value());
  REQUIRE(decompressor.finish_decompress().has_value());

  auto pixels = manager.pixels();
  REQUIRE(pixels.size() == reference.pixels().size());
  REQUIRE_FALSE(pixels.empty());
  CHECK(std::equal(pixels.begin(), pixels.end(), reference.pixels().begin()));
  pixels[0] ^= 0xff;
  CHECK(manager.pixels()[0] == pixels[0]);
  REQUIRE(manager.init().has_value());
  CHECK(std::equal(manager.pixels().begin(), manager.pixels().end(),
                   reference.pixels().begin()));
}

TEST_CASE("PixelMgr reports file and JPEG header errors") {
  img::PixelMgr missing{""};
  auto result = missing.init();
  REQUIRE_FALSE(result.has_value());
  CHECK(result.error().type == img::PixelMgrError::FileOpenInitError);
  CHECK(missing.pixels().empty());

  img::PixelMgr invalid{__FILE__};
  result = invalid.init();
  REQUIRE_FALSE(result.has_value());
  CHECK(result.error().type == img::PixelMgrError::DecompressorInitError);
  CHECK(invalid.pixels().empty());
}
