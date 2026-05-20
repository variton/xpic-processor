
#include <cstdlib>
#include <env_mgr.h>

namespace platform {

/**
 * Return the global EnvMgr instance.
 *
 * Uses a function-local static to implement a singleton.
 * Since C++11, initialization is thread-safe.
 */
EnvMgr &EnvMgr::get_instance() noexcept {
  static EnvMgr instance;
  return instance;
}

/**
 * Look up an environment variable by name.
 *
 * Lookup order:
 * 1. Check the internal cache first
 * 2. If not cached, query the process environment with std::getenv()
 * 3. Cache successful lookups for future calls
 *
 * @param name Name of the environment variable
 * @return The variable value on success
 * @return EnvMgrErrorInfo if the variable does not exist
 *
 * Notes:
 * - This version correctly handles empty-string environment values.
 * - A missing cache entry is represented by std::nullopt, not by "".
 */
tl::expected<std::string, EnvMgrErrorInfo>
EnvMgr::get_env_var(const char *name) {
  // Normalize the key for cache storage/lookup
  const std::string key{name};

  // Fast path: return cached value if present
  if (auto cached = cache_.get(key)) {
    return *cached;
  }

  // Cache miss: query the process environment
  if (const char *envvar = std::getenv(name)) {
    const std::string value{envvar};

    // Store resolved value so future lookups avoid getenv()
    cache_.add(key, value);
    return value;
  }

  // Variable does not exist in the environment
  return tl::unexpected(
      EnvMgrErrorInfo{EnvMgrError::NoEnvVar, "No env var found\n"});
}

/**
 * Construct an EnvMgr with an empty cache.
 *
 * Intended to be used only through get_instance().
 */
EnvMgr::EnvMgr() : cache_{} {}

} // namespace platform
