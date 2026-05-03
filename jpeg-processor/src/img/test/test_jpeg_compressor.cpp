#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>

#include <jpeg_compressor.h>

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
