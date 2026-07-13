
#include <mgr.h>
#include <xclock.h>
#include <thread>
#include <fmt/core.h>

//[[expected::runtime]]
int main(int argc, char **argv) {
  if (argc == 1) {
    fmt::println("sequence missing argument");
    return EXIT_FAILURE;
  }

  using namespace std::chrono_literals;
  obs::micro_c clock;
  clock();
  //quality check to gather picture metric 
  std::this_thread::sleep_for(500ms);
  clock();
  fmt::println("time elasped: {}", clock.elapsed());
  return 0;
}
