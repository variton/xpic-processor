
add_executable(sequence-ssim
    sequence.cpp
)

target_include_directories(sequence-ssim PUBLIC
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/quality>
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/img>
  $<BUILD_INTERFACE:${tcxx_SOURCE_DIR}/include/core>
  $<BUILD_INTERFACE:${tcxx_SOURCE_DIR}/include/obs>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(sequence-ssim PRIVATE
                      img
                      quality 
                      fmt::fmt)

target_link_options(sequence-ssim PRIVATE 
                    $<$<CONFIG:Debug>:--coverage>)

target_compile_options(sequence-ssim PRIVATE 
                       -Wall -Wextra -pedantic
                       $<$<CONFIG:Debug>:--coverage -Og -g>
                       $<$<CONFIG:Release>:
                            -O3 
                            -DNDEBUG
                            -march=native
                            -mtune=native
                            -funroll-loops
                            -fomit-frame-pointer>)
