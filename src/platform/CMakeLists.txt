
add_library(platform SHARED 
            cache.cpp
            env_mgr.cpp)

target_include_directories(platform PUBLIC
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/platform>
  $<BUILD_INTERFACE:${tcxx_SOURCE_DIR}/include/err>
  $<BUILD_INTERFACE:${tcxx_SOURCE_DIR}/include/core>
  $<BUILD_INTERFACE:${tcxx_SOURCE_DIR}/include/topology>
  $<INSTALL_INTERFACE:include>
)

target_link_options(platform PRIVATE 
                    $<$<CONFIG:Debug>:--coverage>)

target_compile_options(platform PRIVATE 
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
