
add_library(metric SHARED
            pnsr_mgr.cpp)

target_include_directories(metric PUBLIC
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/metric>
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/img>
  $<BUILD_INTERFACE:${tcxx_SOURCE_DIR}/include/err>
  $<BUILD_INTERFACE:${tcxx_SOURCE_DIR}/include/core>
  $<BUILD_INTERFACE:${tcxx_SOURCE_DIR}/include/topology>
  $<INSTALL_INTERFACE:include>
)

# target_link_libraries(img PRIVATE 
#                       fio
#                       JPEG::JPEG)

target_link_options(metric PRIVATE 
                    $<$<CONFIG:Debug>:--coverage>)

target_compile_options(metric PRIVATE 
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
