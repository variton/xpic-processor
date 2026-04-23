#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>

#include <img_hdr.h>

TEST_CASE("ImgHdr blend") {
  std::string input{"/home/cxx-core/rc/sample.jpg"};
  std::string output{"/home/cxx-core/test-results/sample-out.jpg"};
  img::ImgHdr hdr{input, output};
  auto ret = hdr.blend(100);
  if (ret)
    CHECK(true);
}

TEST_CASE("ImgHdr open input failure") {
  std::string input{"$ùoel"};
  std::string output{"/home/cxx-core/test-results/sample-out.jpg"};
  img::ImgHdr hdr{input, output};
  auto ret = hdr.blend(100);
  if (!ret) {
    CHECK(ret.error().type == img::ImageError::OpenFileError);
    CHECK(ret.error().message == "Failed to open input file\n");
  }
}

TEST_CASE("ImgHdr open output failure") {
  std::string input{"/home/cxx-core/rc/sample.jpg"};
  std::string output{"xxxx/output.jpg"};
  img::ImgHdr hdr{input, output};
  auto ret = hdr.blend(100);
  if (!ret) {
    CHECK(ret.error().type == img::ImageError::OpenFileError);
    CHECK(ret.error().message == "Failed to open output file\n");
  }
}
