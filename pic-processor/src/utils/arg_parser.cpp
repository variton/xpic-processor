#include <algorithm>
#include <arg_parser.h>
#include <sstream>

namespace {

/**
 * Remove leading and trailing whitespace from a string.
 *
 * Useful for sanitizing command-line values such as file paths that may
 * accidentally include surrounding spaces.
 */
inline std::string trim(const std::string &s) {
  auto start = std::find_if_not(
      s.begin(), s.end(), [](unsigned char c) { return std::isspace(c); });

  auto end = std::find_if_not(s.rbegin(), s.rend(), [](unsigned char c) {
               return std::isspace(c);
             }).base();

  return (start < end) ? std::string(start, end) : std::string();
}

/**
 * Convert a CLI11 parse error into a formatted string.
 *
 * CLI11 prints its formatted diagnostics through app.exit(...), so this helper
 * temporarily redirects std::cerr into a string stream and returns the result.
 *
 * Note:
 * - This is fine for a small CLI tool
 * - It is not ideal in multi-threaded code because std::cerr is global
 */
std::string format_cli_error(CLI::App &app, const CLI::ParseError &e) {
  std::ostringstream oss;
  auto *old = std::cerr.rdbuf(oss.rdbuf());
  app.exit(e);
  std::cerr.rdbuf(old);
  return oss.str();
}

} // namespace

namespace utils {

/**
 * Construct the parser and define supported CLI options once.
 *
 * Doing this in the constructor avoids re-registering the same options every
 * time parse_args() is called.
 */
ArgParser::ArgParser(const std::string &app_name) noexcept
    : opts_{}, app_{app_name} {

  // Required input image path
  app_.add_option("-i,--input", opts_.input, "Input image")
      ->required()
      ->transform([](const std::string &s) { return ::trim(s); });

  // Required output image path
  app_.add_option("-o,--output", opts_.output, "Output image")
      ->required()
      ->transform([](const std::string &s) { return ::trim(s); });

  // Additional help text shown at the bottom of CLI11 help/error output
  app_.footer(
      "Syntax:\n"
      "  deinterlacer -i <input> -o <output>\n"
      "Example:\n"
      "  deinterlacer -i rc/interlaced.jpg -o output/deinterlaced.jpg\n");
}

/**
 * Destructor
 *
 * No manual cleanup is needed; members manage their own resources.
 */
ArgParser::~ArgParser() {}

/**
 * Parse command-line arguments into the Options structure.
 *
 * @param argc Number of command-line arguments
 * @param argv Argument vector
 * @return Parsed options on success, or structured parse error on failure
 *
 * Error strategy:
 * - CLI11 throws CLI::ParseError for invalid input
 * - We catch that and convert it into tl::expected failure
 */
tl::expected<Options, ArgParserErrorInfo>
ArgParser::parse_args(int argc, char *argv[]) noexcept {
  try {
    // Parse argv and populate bound option targets inside opts_
    app_.parse(argc, argv);
    return opts_;
  } catch (const CLI::ParseError &e) {
    return tl::unexpected(ArgParserErrorInfo{ArgParserError::ParseError,
                                             ::format_cli_error(app_, e)});
  }
}

} // namespace utils
