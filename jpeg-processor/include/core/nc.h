/**
 * @file nc.h
 * @brief Provides a non-copyable CRTP base class.
 */

#ifndef NC_H
#define NC_H

#include <concepts>
#include <type_traits>

namespace core {

/**
 * @brief Non-copyable CRTP base class.
 *
 * NC (NonCopyable) is a utility base class that disables
 * copy and copy assignment in derived classes.
 *
 * Intended to be used via inheritance:
 *
 * @code
 * class MyType : public NCNM<MyType> { ... };
 * @endcode
 *
 * @tparam T Derived type.
 */
template <typename T> class NC {
protected:
  /**
   * @brief Default protected constructor.
   */
  NC() = default;

  /**
   * @brief Default protected destructor.
   */
  ~NC() = default;

public:
  NC(const NC &) = delete;            ///< Copy construction disabled.
  NC &operator=(const NC &) = delete; ///< Copy assignment disabled.
};

} // namespace core

#endif // NC_H
