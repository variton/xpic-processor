
add_executable(test_pnsr_mgr
               test_pnsr_mgr.cpp)

target_include_directories(test_pnsr_mgr PUBLIC
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/img>
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/metric>
  $<BUILD_INTERFACE:${tcxx_SOURCE_DIR}/include/err>
  $<BUILD_INTERFACE:${tcxx_SOURCE_DIR}/include/core>
  $<BUILD_INTERFACE:${tcxx_SOURCE_DIR}/include/topology>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(test_pnsr_mgr PRIVATE
                      fio
                      metric
                      img
                      platform
                      doctest::doctest
                      $<$<CONFIG:Debug>:--coverage>)

target_compile_options(test_pnsr_mgr PRIVATE
                       -Wall -Wextra -pedantic
                       $<$<CONFIG:Debug>:--coverage -Og -g>
                       $<$<CONFIG:Release>:-O3 -DNDEBUG>)

add_test(NAME test_pnsr_mgr COMMAND $<TARGET_FILE:test_pnsr_mgr>)

set_tests_properties(test_pnsr_mgr PROPERTIES
  ENVIRONMENT "RC=${TRC};OUT=${TOUT}"
)

#=======================================================================
add_executable(test_ssim_mgr
               test_ssim_mgr.cpp)

target_include_directories(test_ssim_mgr PUBLIC
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/img>
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/metric>
  $<BUILD_INTERFACE:${tcxx_SOURCE_DIR}/include/err>
  $<BUILD_INTERFACE:${tcxx_SOURCE_DIR}/include/core>
  $<BUILD_INTERFACE:${tcxx_SOURCE_DIR}/include/topology>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(test_ssim_mgr PRIVATE
                      fio
                      metric
                      img
                      platform
                      doctest::doctest
                      $<$<CONFIG:Debug>:--coverage>)

target_compile_options(test_ssim_mgr PRIVATE
                       -Wall -Wextra -pedantic
                       $<$<CONFIG:Debug>:--coverage -Og -g>
                       $<$<CONFIG:Release>:-O3 -DNDEBUG>)

add_test(NAME test_ssim_mgr COMMAND $<TARGET_FILE:test_ssim_mgr>)

set_tests_properties(test_ssim_mgr PROPERTIES
  ENVIRONMENT "RC=${TRC};OUT=${TOUT}"
)
