
add_executable(extract-pixel-buffer-libjpeg
    fetch_pixel_buffer.cpp
)

target_include_directories(extract-pixel-buffer-libjpeg PUBLIC
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/metric>
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/img>
  $<BUILD_INTERFACE:${tcxx_SOURCE_DIR}/include/core>
  $<BUILD_INTERFACE:${tcxx_SOURCE_DIR}/include/obs>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(extract-pixel-buffer-libjpeg PRIVATE
                      img
                      metric 
                      fmt::fmt)

target_link_options(extract-pixel-buffer-libjpeg PRIVATE 
                    $<$<CONFIG:Debug>:--coverage>)

target_compile_options(extract-pixel-buffer-libjpeg PRIVATE 
                       -Wall -Wextra -pedantic
                       $<$<CONFIG:Debug>:--coverage -Og -g>
                       $<$<CONFIG:Release>:
                            -O3 
                            -DNDEBUG
                            -march=native
                            -mtune=native
                            -funroll-loops
                            -fomit-frame-pointer>)

#============================================================
