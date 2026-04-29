
#ifndef ERROR_INFO_H
#define ERROR_INFO_H

#include <string>
#include <type_traits>

/**
 * @file error_info.h
 * @brief Defines a generic error container with type and message.
 */

namespace err {

/**
 * @brief Generic structure representing an error with a type and message.
 *
 * @tparam ErrorType Type used to identify the error (e.g., enum or enum class).
 *
 * @details
 * `ErrorInfo` is a lightweight data container that associates a typed error
 * identifier with a human-readable message. It is intended for use in
 * error reporting, result types, and logging.
 *
 * @note No constraints are enforced on @p ErrorType, but it is expected
 *       to be copyable.
 */
template <typename ErrorType> struct ErrorInfo {
  /** @brief Error identifier or category. */
  ErrorType type;

  /** @brief Human-readable error description. */
  std::string message;
};

} // namespace err

#endif
