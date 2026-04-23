
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>

#include <corety.h>
#include <nc.h>
#include <ncnm.h>

template <typename T> using Default = core::NCNM<T>;

template <typename T> using Movable = core::NC<T>;

struct Sample : public Default<Sample> {};

struct Decoy : public Movable<Decoy> {};

TEST_CASE("Non copyble non moveable class") {
  static_assert(topology::NonCopyableNonMovable<Sample>);
  CHECK(true);
}

TEST_CASE("Non copyble class") {
  static_assert(topology::NonCopyable<Decoy>);
  CHECK(true);
}
