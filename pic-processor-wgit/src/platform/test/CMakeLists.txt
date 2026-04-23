
add_executable(test_env_mgr
    test_env_mgr.cpp
)

target_include_directories(test_env_mgr PUBLIC
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/platform>
  $<INSTALL_INTERFACE:include>
)



target_link_libraries(test_env_mgr PRIVATE
                      platform
                      doctest::doctest
                      fmt::fmt)


target_link_options(test_env_mgr PRIVATE
                    $<$<CONFIG:Debug>:--coverage>)

target_compile_options(test_env_mgr PRIVATE
                       -Wall -Wextra -pedantic
                       $<$<CONFIG:Debug>:--coverage -Og -g>
                       $<$<CONFIG:Release>:-O3 -DNDEBUG>)

add_test(NAME test_env_mgr COMMAND $<TARGET_FILE:test_env_mgr>)

set_tests_properties(test_env_mgr PROPERTIES
  ENVIRONMENT "RC=${TRC}"
)

#=====================================================================

add_executable(test_cache
    test_cache.cpp
)

target_include_directories(test_cache PUBLIC
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/>
  $<INSTALL_INTERFACE:include>
)


target_link_libraries(test_cache PRIVATE
                      platform
                      doctest::doctest
                      fmt::fmt)

target_link_options(test_cache PRIVATE
                    $<$<CONFIG:Debug>:--coverage>)


target_compile_options(test_cache PRIVATE
                       -Wall -Wextra -pedantic
                       $<$<CONFIG:Debug>:--coverage -Og -g>
                       $<$<CONFIG:Release>:-O3 -DNDEBUG>)

add_test(NAME test_cache COMMAND $<TARGET_FILE:test_cache>)

set_tests_properties(test_cache PROPERTIES
  ENVIRONMENT "RC=${TRC}"
)
