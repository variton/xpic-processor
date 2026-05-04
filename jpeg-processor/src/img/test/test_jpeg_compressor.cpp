#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>

#include <string>

#include <filehandler.h>
#include <inputimg.h>
#include <jpeg_compressor.h>
#include <jpeg_decompressor.h>

TEST_CASE("JpegCompressor compression failure") {
  using namespace img;
  JpegCompressor compressor{};
  auto ret_compress = compressor.compress();

  if (!ret_compress) {
    CHECK(ret_compress.error().type == JpegCompressorError::NotInitialized);
    CHECK(ret_compress.error().message == "JPEG compression not initialized\n");
  }
}

TEST_CASE("JpegCompressor finish compression failure") {
  using namespace img;
  JpegCompressor compressor{};
  auto ret_compress = compressor.finish_compress();

  if (!ret_compress) {
    CHECK(ret_compress.error().type == JpegCompressorError::NotInitialized);
    CHECK(ret_compress.error().message == "JPEG compression not initialized\n");
  }
}

TEST_CASE("JpegCompressor init compression") {
  using namespace img;
  std::string input{"/home/cxx-core/rc/sample.jpg"};
  std::string output{"/home/cxx-core/test-results/sample-out.jpg"};

  auto input_file = fio::FileHandler::open(input.data(), "rb");
  CHECK(input_file.has_value());
  auto output_file = fio::FileHandler::open(output.data(), "wb");
  CHECK(output_file.has_value());

  JpegDecompressor decompressor{};
  auto ret_init_decompressor = decompressor.init(input_file.value().get());
  CHECK(ret_init_decompressor.has_value());
  auto ret_decompress = decompressor.decompress();
  CHECK(ret_decompress.has_value());
  InputImg inputimg{decompressor.cinfo()};

  JpegCompressor compressor{};
  auto ret_init_compressor =
      compressor.init(output_file.value().get(), inputimg, 85);
  CHECK(ret_init_compressor.has_value());
}

TEST_CASE("JpegCompressor init compression failed") {
  using namespace img;
  InputImg inputimg{};
  JpegCompressor c{};
  auto ret_init_compressor = c.init(nullptr, inputimg, 85);
  if (!ret_init_compressor) {
    CHECK(ret_init_compressor.error().type ==
          JpegCompressorError::InitCompressionError);
    CHECK(ret_init_compressor.error().message ==
          "JPEG compression init failed\n");
  }
}
