#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>

#include <string>

#include <filehandler.h>
#include <fmt/core.h>
#include <inputimg.h>
#include <jpeg_decoder.h>
#include <jpeg_decompressor.h>

TEST_CASE("Decode OK") {
  std::string rc{"/home/cxx-core/rc"};
  using namespace img;
  std::string input{rc + "/ur.jpg"};
  auto input_file = fio::FileHandler::open(input.data(), "rb");
  // jpeg decompression creation ============================================
  JpegDecompressor decompressor{};
  auto ret_init_decompressor = decompressor.init(input_file.value().get());
  auto ret_decompress = decompressor.decompress();
  // image object creation ==================================================
  InputImg inputimg{decompressor.cinfo()};
  // jpeg decoder ===========================================================
  JpegDecoder original{inputimg.width, inputimg.height, inputimg.components};
  auto ret_decode = original.decode(decompressor);
  CHECK(ret_decode.has_value());
}

TEST_CASE("Pixels OK") {
  std::string rc{"/home/cxx-core/rc"};
  using namespace img;
  std::string input{rc + "/ur.jpg"};
  auto input_file = fio::FileHandler::open(input.data(), "rb");
  // jpeg decompressor creation =============================================
  JpegDecompressor decompressor{};
  auto ret_init_decompressor = decompressor.init(input_file.value().get());
  auto ret_decompress = decompressor.decompress();
  // image object creation ==================================================
  InputImg inputimg{decompressor.cinfo()};
  // jpeg decoder ===========================================================
  JpegDecoder original{inputimg.width, inputimg.height, inputimg.components};
  auto ret_decode = original.decode(decompressor);
  CHECK(ret_decode.has_value());
  auto pixels = original.pixels();
  // auto * data = pixels.data();
  CHECK(pixels.size() ==
        inputimg.width * inputimg.height * inputimg.components);
}
