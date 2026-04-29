
add_executable(test_arg_parser
    test_arg_parser.cpp
)

target_include_directories(test_arg_parser PUBLIC
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/utils>
  $<INSTALL_INTERFACE:include>
)



target_link_libraries(test_arg_parser PRIVATE
                      utils 
                      doctest::doctest
                      fmt::fmt)


target_link_options(test_arg_parser PRIVATE
                    $<$<CONFIG:Debug>:--coverage>)

target_compile_options(test_arg_parser PRIVATE
                       -Wall -Wextra -pedantic
                       $<$<CONFIG:Debug>:--coverage -Og -g>
                       $<$<CONFIG:Release>:-O3 -DNDEBUG>)

add_test(NAME test_arg_parser COMMAND $<TARGET_FILE:test_arg_parser>)

set_tests_properties(test_arg_parser PROPERTIES
  ENVIRONMENT "RC=${TRC}"
)
