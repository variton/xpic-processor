
#include <mgr.h>
#include <fmt/core.h>
#include <runtime.h>

MICRO_CLOCK_RUNTIME_MAIN {
  clock();
  //quality check to gather picture metric 
  clock();
  fmt::println("time elasped: {}", clock.elapsed());
  return 0;
}
