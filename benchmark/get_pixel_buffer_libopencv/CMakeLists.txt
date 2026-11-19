
add_executable(extract-pixel-buffer-libopencv
    fetch_pixel_buffer.cpp
)

target_include_directories(extract-pixel-buffer-libopencv PUBLIC
  $<BUILD_INTERFACE:${tcxx_SOURCE_DIR}/include/obs>
  $<INSTALL_INTERFACE:include>
  ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(extract-pixel-buffer-libopencv PRIVATE
                      img
                      metric 
                      fmt::fmt
                      ${OpenCV_LIBS})

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
