#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>

#include <arg_parser.h>

TEST_CASE("ArgParser parse") {
  utils::ArgParser local_parser{"Test ArgParser-parser"};
  const char *args[] = {"./decoy", "-i rc/interlaced.jpg",
                        "-o deinterlaced.jpg"};
  size_t length = sizeof(args) / sizeof(args[0]);

  auto ret = local_parser.parse_args(length, const_cast<char **>(args));
  if (ret) {
    auto options = ret.value();
    CHECK(options.input == "rc/interlaced.jpg");
    CHECK(options.output == "deinterlaced.jpg");
  }
}

TEST_CASE("ArgParser parse fail") {
  utils::ArgParser local_parser{"Test ArgParser-parser-fail"};
  const char *args[] = {"./decoy"};
  size_t length = sizeof(args) / sizeof(args[0]);

  auto ret = local_parser.parse_args(length, const_cast<char **>(args));
  if (!ret) {
    CHECK(ret.error().type == utils::ArgParserError::ParseError);
    CHECK(ret.error().message ==
          "--input is required\nRun with --help for more information.\n");
  }
}
