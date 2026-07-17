
#include <mgr.h>
#include <xclock.h>
#include <thread>
#include <fmt/core.h>

//todo: should be moved to a tcxx23 and tcxx
//define a runtime for all sorts of clocks
#define MICRO_CLOCK_RUNTIME_MAIN                                      \
    static int clock_runtime_main(                                      \
        int argc, char** argv, obs::micro_c& clock);                    \
                                                                         \
    int main(int argc, char** argv) {                                   \
        if (argc == 1) {                                                \
            fmt::println("sequence missing argument");                  \
            return EXIT_FAILURE;                                        \
        }                                                               \
                                                                         \
        obs::micro_c clock;                                             \
        return clock_runtime_main(argc, argv, clock);                    \
    }                                                                   \
                                                                         \
    static int clock_runtime_main(                                      \
        int argc, char** argv, [[maybe_unused]] obs::micro_c& clock)


MICRO_CLOCK_RUNTIME_MAIN {
  //using namespace std::chrono_literals;
  clock();
  //quality check to gather picture metric 
  //std::this_thread::sleep_for(500ms);
  clock();
  fmt::println("time elasped: {}", clock.elapsed());
  return 0;
}
