
#ifndef ARG_PARSER_H
#define ARG_PARSER_H

#include <string>

#include <CLI/CLI.hpp>

#include <error_info.h>
#include <errty.h>
#include <ncnm.h>
#include <tl/expected.hpp>

namespace utils {

/**
 * @brief Alias for a non-copyable, non-movable base class.
 *
 * @tparam T Derived type.
 */
template <typename T> using Default = core::NCNM<T>;

/**
 * @brief Error codes produced by @ref ArgParser.
 */
enum class ArgParserError {
  /** @brief Required arguments are missing. */
  MissingArgs,
  /** @brief Help option was requested. */
  HelpRequested,
  /** @brief Invalid argument format. */
  InvalidFormat,
  /** @brief Argument parsing failed. */
  ParseError
};

/**
 * @brief Structured error information for argument parsing.
 */
using ArgParserErrorInfo = err::ErrorInfo<ArgParserError>;

static_assert(topology::ErrorInfoTy<ArgParserErrorInfo>);

/**
 * @brief Parsed command-line options.
 */
struct Options {
  /** @brief Input path. */
  std::string input;

  /** @brief Output path. */
  std::string output;
};

/**
 * @brief Command-line argument parser.
 *
 * @details
 * Wraps @c CLI::App to parse command-line arguments into an @ref Options
 * structure. The class is non-copyable and non-movable.
 */
class ArgParser : Default<ArgParser> {

public:
  /**
   * @brief Constructs the parser with an application name.
   *
   * @param app_name Name of the application.
   */
  explicit ArgParser(const std::string &app_name) noexcept;

  /**
   * @brief Destroys the parser.
   */
  ~ArgParser();

  /**
   * @brief Parses command-line arguments.
   *
   * @param argc Argument count.
   * @param argv Argument vector.
   * @return `tl::expected` containing parsed @ref Options on success, or
   *         @ref ArgParserErrorInfo on failure.
   */
  tl::expected<Options, ArgParserErrorInfo> parse_args(int argc,
                                                       char *argv[]) noexcept;

private:
  /** @brief Parsed options. */
  Options opts_;

  /** @brief Underlying CLI application instance. */
  CLI::App app_;
};

} // namespace utils

#endif
