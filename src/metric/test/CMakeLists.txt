
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

# add_executable(test_jpeg_compressor
#                test_jpeg_compressor.cpp)

# target_include_directories(test_jpeg_compressor PUBLIC
#   $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/img>
#   $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/fio>
#   $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/core>
#   $<INSTALL_INTERFACE:include>
# )

# target_link_libraries(test_jpeg_compressor PRIVATE
#                       img
#                       fio
#                       doctest::doctest
#                       $<$<CONFIG:Debug>:--coverage>)

# target_compile_options(test_jpeg_compressor PRIVATE
#                        -Wall -Wextra -pedantic
#                        $<$<CONFIG:Debug>:--coverage -Og -g>
#                        $<$<CONFIG:Release>:-O3 -DNDEBUG>)

# add_test(NAME test_jpeg_compressor COMMAND $<TARGET_FILE:test_jpeg_compressor>)

# set_tests_properties(test_jpeg_compressor PROPERTIES
#   ENVIRONMENT "RC=${TRC};OUT=${TOUT}"
# )
