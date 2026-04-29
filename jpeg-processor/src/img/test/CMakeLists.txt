
add_executable(test_img_hdr
               test_img_hdr.cpp)

target_include_directories(test_img_hdr PUBLIC
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/img>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(test_img_hdr PRIVATE
                      img
                      platform
                      doctest::doctest
                      fmt::fmt
                      $<$<CONFIG:Debug>:--coverage>)

target_compile_options(test_img_hdr PRIVATE
                       -Wall -Wextra -pedantic
                       $<$<CONFIG:Debug>:--coverage -Og -g>
                       $<$<CONFIG:Release>:-O3 -DNDEBUG>)

add_test(NAME test_img_hdr COMMAND $<TARGET_FILE:test_img_hdr>)

set_tests_properties(test_img_hdr PROPERTIES
  ENVIRONMENT "RC=${TRC};OUT=${TOUT}"
)
