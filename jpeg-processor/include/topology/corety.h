
#ifndef CORETY_H
#define CORETY_H

#include <concepts>
#include <type_traits>

namespace topology {

/**
 * @brief Concept for types that are neither copyable nor movable.
 *
 * @details
 * Satisfied if and only if the type `T`:
 * - is not copy constructible
 * - is not copy assignable
 * - is not move constructible
 * - is not move assignable
 *
 * This concept can be used to:
 * - Constrain templates requiring strict ownership semantics
 * - Assert invariants in generic code
 *
 * @tparam T Type to evaluate.
 *
 * @par Example
 * @code
 * template <core::NonCopyableNonMovable T>
 * void process(T& obj);
 * @endcode
 */
template <typename T>
concept NonCopyableNonMovable =
    !std::is_copy_constructible_v<T> && !std::is_copy_assignable_v<T> &&
    !std::is_move_constructible_v<T> && !std::is_move_assignable_v<T>;

/**
 * @brief Concept for types that are not copyable.
 *
 * @details
 * Satisfied if and only if the type `T`:
 * - is not copy constructible
 * - is not copy assignable
 *
 * This concept can be used to:
 * - Constrain templates requiring strict ownership semantics
 * - Assert invariants in generic code
 *
 * @tparam T Type to evaluate.
 *
 * @par Example
 * @code
 * template <utils::NonCopyable T>
 * void process(T& obj);
 * @endcode
 */
template <typename T>
concept NonCopyable =
    !std::is_move_constructible_v<T> && !std::is_move_assignable_v<T>;

} // namespace topology

#endif // CORETY_H
