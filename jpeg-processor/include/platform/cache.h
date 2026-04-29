
#ifndef CACHE_H
#define CACHE_H

#include <cstddef>
#include <map>
#include <ncnm.h>
#include <optional>
#include <string>

namespace platform {

/**
 * @brief Alias for a non-copyable, non-movable base class.
 *
 * @tparam T Derived type.
 */
template <typename T> using Default = core::NCNM<T>;

/**
 * @brief Simple key-value cache for string data.
 *
 * @details
 * Provides a lightweight wrapper around @c std::map for storing and
 * retrieving string-based key-value pairs. The class is non-copyable
 * and non-movable.
 *
 * @note This class is not thread-safe.
 */
class Cache : public Default<Cache> {

public:
  /**
   * @brief Constructs an empty cache.
   */
  Cache();

  /**
   * @brief Destroys the cache.
   */
  ~Cache();

  /**
   * @brief Inserts or updates a key-value pair.
   *
   * @param key Key to insert or update.
   * @param value Value associated with the key.
   */
  void add(const std::string &key, const std::string &value);

  /**
   * @brief Retrieves the value associated with a key.
   *
   * @param key Key to look up.
   * @return Associated value if found, otherwise an empty string.
   *
   * @note An empty string does not distinguish between a missing key and
   *       a stored empty value.
   */
  std::optional<std::string> get(const std::string &key) const;

  /**
   * @brief Returns the number of stored entries.
   *
   * @return Number of key-value pairs.
   */
  size_t size() const noexcept;

private:
  /** @brief Internal storage for key-value pairs. */
  std::map<std::string, std::string> cache_;
};

} // namespace platform

#endif
