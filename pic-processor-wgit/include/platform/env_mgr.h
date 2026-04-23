#ifndef ENV_MGR_H
#define ENV_MGR_H

#include <cstdlib>
#include <string>

#include <cache.h>
#include <error_info.h>
#include <errty.h>
#include <ncnm.h>
#include <tl/expected.hpp>

namespace platform {

/**
 * @brief Alias for a non-copyable, non-movable base class.
 *
 * @tparam T Derived type.
 */
template <typename T> using Default = core::NCNM<T>;

/**
 * @brief Error codes produced by @ref EnvMgr.
 */
enum class EnvMgrError {
  /** @brief The requested environment variable was not found. */
  NoEnvVar,

  /** @brief The environment variable value is not of the expected type. */
  InvalidEnvVarType,
};

/**
 * @brief Structured error information for @ref EnvMgr operations.
 */
using EnvMgrErrorInfo = err::ErrorInfo<EnvMgrError>;

static_assert(topology::ErrorInfoTy<EnvMgrErrorInfo>);

/**
 * @brief Singleton manager for reading and caching environment variables.
 *
 * @details
 * `EnvMgr` provides access to process environment variables and caches
 * previously resolved values to avoid repeated calls to @c std::getenv.
 *
 * The class is non-copyable and non-movable.
 */
class EnvMgr : public Default<EnvMgr> {
public:
  /**
   * @brief Returns the singleton instance.
   *
   * @return Reference to the global environment manager instance.
   */
  static EnvMgr &get_instance() noexcept;

  /**
   * @brief Returns the value of an environment variable.
   *
   * @param name Null-terminated environment variable name.
   * @return `tl::expected` containing the variable value on success, or
   *         @ref EnvMgrErrorInfo on failure.
   *
   * @retval EnvMgrError::NoEnvVar The requested variable does not exist.
   */
  tl::expected<std::string, EnvMgrErrorInfo> get_env_var(const char *name);

private:
  /**
   * @brief Constructs the environment manager.
   */
  EnvMgr();

  /** @brief Cache of resolved environment variable values. */
  Cache cache_;
};

} // namespace platform

#endif
