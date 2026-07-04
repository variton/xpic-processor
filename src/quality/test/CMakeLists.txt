
add_executable(test_mgr
               test_mgr.cpp)

target_include_directories(test_mgr PUBLIC
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/quality>
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/metric>
  $<BUILD_INTERFACE:${tcxx_SOURCE_DIR}/include/err>
  $<BUILD_INTERFACE:${tcxx_SOURCE_DIR}/include/core>
  $<BUILD_INTERFACE:${tcxx_SOURCE_DIR}/include/topology>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(test_mgr PRIVATE
                      fio
                      metric
                      img
                      platform
                      quality
                      doctest::doctest
                      $<$<CONFIG:Debug>:--coverage>)

target_compile_options(test_mgr PRIVATE
                       -Wall -Wextra -pedantic
                       $<$<CONFIG:Debug>:--coverage -Og -g>
                       $<$<CONFIG:Release>:-O3 -DNDEBUG>)

add_test(NAME test_mgr COMMAND $<TARGET_FILE:test_mgr>)

set_tests_properties(test_mgr PROPERTIES
  ENVIRONMENT "RC=${TRC};OUT=${TOUT}"
)
