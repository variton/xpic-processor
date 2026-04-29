#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>

#include <cache.h>
#include <env_mgr.h>

TEST_CASE("Cache add") {
  auto &local = platform::EnvMgr::get_instance();
  auto rc = local.get_env_var("RC");
  if (rc) {
    platform::Cache local_cache{};
    CHECK(local_cache.size() == 0);
    local_cache.add("RC", rc.value());
    CHECK(local_cache.size() == 1);
  }
}

TEST_CASE("Cache update") {
  auto &local = platform::EnvMgr::get_instance();
  auto rc = local.get_env_var("RC");
  if (rc) {
    platform::Cache local_cache{};
    local_cache.add("RC", rc.value());
    auto rc_val = local_cache.get("RC");
    CHECK(rc_val == "/home/cxx-core/rc");
    CHECK(local_cache.size() == 1);
    local_cache.add("RC", "/home/spectrum");
    CHECK(local_cache.size() == 1);
    rc_val = local_cache.get("RC");
    CHECK(rc_val == "/home/spectrum");
  }
}

TEST_CASE("Cache get") {
  auto &local = platform::EnvMgr::get_instance();
  auto rc = local.get_env_var("RC");
  if (rc) {
    platform::Cache local_cache{};
    local_cache.add("RC", rc.value());
    auto rc_val = local_cache.get("RC");
    CHECK(rc_val == "/home/cxx-core/rc");
    CHECK(local_cache.size() == 1);
  }
}

TEST_CASE("Cache get fail") {
  platform::Cache local_cache{};
  auto rc_val = local_cache.get("RC");
  CHECK(rc_val == std::nullopt);
  CHECK(local_cache.size() == 0);
}
