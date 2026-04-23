
#include <arg_parser.h>
#include <img_hdr.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

namespace {
template <typename T> int handle_error(const T &err, spdlog::logger *logger) {
  if (!err) {
    logger->error(" {} ", err.error().message);
    return EXIT_FAILURE;
  }
  return 0;
}
} // namespace

int main(int argc, char **argv) {
  auto logger = spdlog::basic_logger_mt("deinterlacer_logger",
                                        "logs/deinterlacer.log", true);

  utils::ArgParser parser("deinterlacer");
  auto ret_parse = parser.parse_args(argc, argv);
  if (int ret = ::handle_error(ret_parse, logger.get()); ret != 0)
    return ret;

  auto cli_options = ret_parse.value();
  logger->info("processing image {}", cli_options.input);

  img::ImgHdr hdr{cli_options.input, cli_options.output};
  // the quality of the blending that was choosen is 85%
  auto ret_img = hdr.blend();
  if (int ret = ::handle_error(ret_img, logger.get()); ret != 0)
    return ret;

  auto [width, height] = ret_img.value();
  logger->info("Blended {} x {} deinterlaced image at {}", width, height,
               cli_options.output);

  return 0;
}
