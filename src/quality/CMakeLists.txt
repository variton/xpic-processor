
add_library(quality SHARED
            mgr.cpp)

target_include_directories(quality PUBLIC
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/quality>
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/metric>
  $<BUILD_INTERFACE:${tcxx_SOURCE_DIR}/include/err>
  $<BUILD_INTERFACE:${tcxx_SOURCE_DIR}/include/core>
  $<BUILD_INTERFACE:${tcxx_SOURCE_DIR}/include/topology>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(quality PRIVATE 
                      metric)

target_link_options(quality PRIVATE 
                    $<$<CONFIG:Debug>:--coverage>)

target_compile_options(quality PRIVATE 
                       -Wall -Wextra -pedantic
                       $<$<CONFIG:Debug>:--coverage -Og -g>
                       $<$<CONFIG:Release>:
                            -O3 
                            -DNDEBUG
                            -march=native
                            -mtune=native
                            -funroll-loops
                            -fomit-frame-pointer>)

add_subdirectory(test)
