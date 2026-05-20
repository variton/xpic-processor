#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>

#include <string>

#include <blender.h>
#include <filehandler.h>
#include <inputimg.h>
#include <jpeg_compressor.h>
#include <jpeg_decompressor.h>

TEST_CASE("Blender blend") {
  using namespace img;
  std::string input{"/home/cxx-core/rc/sample.jpg"};
  auto input_file = fio::FileHandler::open(input.data(), "rb");
  CHECK(input_file.has_value());

  std::string output{"/home/cxx-core/test-results/sample-blend-out.jpg"};
  auto output_file = fio::FileHandler::open(output.data(), "wb");
  CHECK(output_file.has_value());
  // decompression ==========================================================
  JpegDecompressor decompressor{};
  auto ret_init_decompressor = decompressor.init(input_file.value().get());
  CHECK(ret_init_decompressor.has_value());
  auto ret_decompress = decompressor.decompress();
  CHECK(ret_decompress.has_value());
  // image object creation ==================================================
  InputImg inputimg{decompressor.cinfo()};
  // compression ============================================================
  JpegCompressor compressor{};
  auto ret_init_compressor =
      compressor.init(output_file.value().get(), inputimg, 90);
  CHECK(ret_init_compressor.has_value());
  auto ret_compression = compressor.compress();
  CHECK(ret_compression.has_value());
  // blending ===============================================================
  Blender blender{inputimg};
  // Generation of deinterlaced output =====================================
  auto ret_blend = blender.blend(compressor, decompressor);
  CHECK(ret_blend.has_value());
}

// todo fix test coverage
// TEST_CASE("Blender blend fails") {
// }
