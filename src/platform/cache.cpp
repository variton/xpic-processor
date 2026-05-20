
#include <cache.h>

namespace platform {

/**
 * Constructor
 *
 * Default-initializes the internal cache container.
 */
Cache::Cache() {}

/**
 * Destructor
 *
 * No explicit cleanup required.
 * The underlying container (likely std::unordered_map or std::map)
 * handles its own resource management.
 */
Cache::~Cache() {}

/**
 * Insert or update a key-value pair in the cache.
 *
 * @param key   Lookup key
 * @param value Value to store
 *
 * Behavior:
 * - If the key does not exist, a new entry is created
 * - If the key already exists, its value is overwritten
 */
void Cache::add(const std::string &key, const std::string &value) {
  cache_[key] = value;
}

std::optional<std::string> Cache::get(const std::string &key) const {
  auto it = cache_.find(key);
  if (it != cache_.end()) {
    return it->second;
  }
  return std::nullopt;
}

/**
 * Get the number of entries in the cache.
 *
 * @return Number of stored key-value pairs
 *
 * noexcept:
 * - Guaranteed not to throw (delegates to container::size)
 */
size_t Cache::size() const noexcept { return cache_.size(); }

} // namespace platform
