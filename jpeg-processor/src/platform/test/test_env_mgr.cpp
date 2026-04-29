#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>

#include <env_mgr.h>

TEST_CASE("EnvMgr get_env_var") {
  auto &local = platform::EnvMgr::get_instance();
  auto rc = local.get_env_var("RC");
  if (rc) {
    CHECK(rc.value() == "/home/cxx-core/rc");
  }
}

TEST_CASE("EnvMgr get_env_var fail") {
  auto &local = platform::EnvMgr::get_instance();
  auto rc = local.get_env_var("XY");
  if (!rc) {
    CHECK(rc.error().type == platform::EnvMgrError::NoEnvVar);
    CHECK(rc.error().message == "No env var found\n");
  }
}
