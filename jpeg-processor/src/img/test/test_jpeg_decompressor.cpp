#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>

#include <string>

#include <filehandler.h>
#include <inputimg.h>
#include <jpeg_decompressor.h>

TEST_CASE("JpegDecompressor compression failure") {
  using namespace img;
  JpegDecompressor decompressor{};
  auto ret_compress = decompressor.decompress();

  if (!ret_compress) {
    CHECK(ret_compress.error().type == JpegDecompressorError::NotInitialized);
    CHECK(ret_compress.error().message ==
          "JPEG decompression not initialized\n");
  }
}

TEST_CASE("JpegCompressor finish compression failure") {
  using namespace img;
  JpegDecompressor decompressor{};
  auto ret_decompress = decompressor.finish_decompress();

  if (!ret_decompress) {
    CHECK(ret_decompress.error().type == JpegDecompressorError::NotInitialized);
    CHECK(ret_decompress.error().message ==
          "JPEG decompression not initialized\n");
  }
}

TEST_CASE("JpegDecompressor init decompression") {
  using namespace img;
  std::string input{"/home/cxx-core/rc/sample.jpg"};

  auto input_file = fio::FileHandler::open(input.data(), "rb");
  CHECK(input_file.has_value());

  JpegDecompressor decompressor{};
  auto ret_init_decompressor = decompressor.init(input_file.value().get());
  CHECK(ret_init_decompressor.has_value());
  auto ret_decompress = decompressor.decompress();
  CHECK(ret_decompress.has_value());
}

// todo fix test coverage
// TEST_CASE("JpegDecompressor init decompression failed") {
//   using namespace img;
//   InputImg inputimg{};
//   inputimg.width = -1;
//   inputimg.height = -1;
//   JpegCompressor c{};
//   auto ret_init_compressor = c.init(nullptr, inputimg, 85);
//   if (!ret_init_compressor) {
//     CHECK(ret_init_compressor.error().type ==
//           JpegCompressorError::InitCompressionError);
//     CHECK(ret_init_compressor.error().message ==
//           "JPEG compression init failed\n");
//   }
// }
