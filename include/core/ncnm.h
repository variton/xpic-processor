/**
 * @file ncnm.h
 * @brief Provides a non-copyable, non-movable CRTP base class.
 */

#ifndef NCNM_H
#define NCNM_H

#include <concepts>
#include <type_traits>

namespace core {

/**
 * @brief Non-copyable, non-movable CRTP base class.
 *
 * NCNM (NonCopyableNonMovable) is a utility base class that disables
 * copy and move construction and assignment in derived classes.
 *
 * Intended to be used via inheritance:
 *
 * @code
 * class MyType : public NCNM<MyType> { ... };
 * @endcode
 *
 * @tparam T Derived type.
 */
template <typename T> class NCNM {
protected:
  /**
   * @brief Default protected constructor.
   */
  NCNM() = default;

  /**
   * @brief Default protected destructor.
   */
  ~NCNM() = default;

public:
  NCNM(const NCNM &) = delete;            ///< Copy construction disabled.
  NCNM &operator=(const NCNM &) = delete; ///< Copy assignment disabled.

  NCNM(NCNM &&) = delete;            ///< Move construction disabled.
  NCNM &operator=(NCNM &&) = delete; ///< Move assignment disabled.
};

} // namespace core

#endif // NCNM_H
