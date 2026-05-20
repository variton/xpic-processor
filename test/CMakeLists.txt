add_executable(test_core
    test_core.cpp
)

target_include_directories(test_core PUBLIC
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/core>
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/topology>
  $<INSTALL_INTERFACE:include>
)

target_link_options(test_core PRIVATE
                    $<$<CONFIG:Debug>:--coverage>)

target_compile_options(test_core PRIVATE 
                       -Wall -Wextra -pedantic
                       $<$<CONFIG:Debug>:--coverage -Og -g>
                       $<$<CONFIG:Release>:-O3 -DNDEBUG>)

add_test(NAME test_core COMMAND $<TARGET_FILE:test_core>)

set_tests_properties(test_core PROPERTIES
  ENVIRONMENT "RC=${TRC}"
)
