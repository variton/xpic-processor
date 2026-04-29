
#ifndef ERRTY_H
#define ERRTY_H

#include <concepts>
#include <string>
#include <type_traits>

namespace topology {

/**
 * @brief Concept for structured error information types.
 *
 * @tparam E Type to validate.
 *
 * @details
 * Satisfied when @p E provides:
 * - a @c type member whose type is an enumeration
 * - a @c message member of type @c std::string
 */
template <typename E>
concept ErrorInfoTy = requires(E e) {
  requires std::is_enum_v<decltype(e.type)>;
  requires std::same_as<decltype(e.message), std::string>;
};

} // namespace topology

#endif
