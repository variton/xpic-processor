
add_executable(test_filehandler
  test_filehandler.cpp
)

target_include_directories(test_filehandler PUBLIC
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/fio>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(test_filehandler PRIVATE 
                      fio
                      doctest::doctest)

target_link_options(test_filehandler PRIVATE
                    $<$<CONFIG:Debug>:--coverage>)

target_compile_options(test_filehandler PRIVATE 
                       -Wall -Wextra -pedantic
                       $<$<CONFIG:Debug>:--coverage -Og -g>
                       $<$<CONFIG:Release>:-O3 -DNDEBUG>)

add_test(NAME test_filehandler COMMAND $<TARGET_FILE:test_filehandler>)
