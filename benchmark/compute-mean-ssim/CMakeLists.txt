
add_executable(sequence-mean-ssim
    sequence.cpp
)

target_include_directories(sequence-mean-ssim PUBLIC
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/metric>
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/img>
  $<BUILD_INTERFACE:${tcxx_SOURCE_DIR}/include/core>
  $<BUILD_INTERFACE:${tcxx_SOURCE_DIR}/include/obs>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(sequence-mean-ssim PRIVATE
                      img
                      metric 
                      fmt::fmt)

target_link_options(sequence-mean-ssim PRIVATE 
                    $<$<CONFIG:Debug>:--coverage>)

target_compile_options(sequence-mean-ssim PRIVATE 
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

add_executable(mt-mean-ssim
    mt.cpp
)

target_include_directories(mt-mean-ssim PUBLIC
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/metric>
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/img>
  $<BUILD_INTERFACE:${tcxx_SOURCE_DIR}/include/core>
  $<BUILD_INTERFACE:${tcxx_SOURCE_DIR}/include/obs>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(mt-mean-ssim PRIVATE
                      img
                      metric
                      fmt::fmt)

target_link_options(mt-mean-ssim PRIVATE
                    $<$<CONFIG:Debug>:--coverage>)

target_compile_options(mt-mean-ssim PRIVATE
                       -Wall -Wextra -pedantic
                       $<$<CONFIG:Debug>:--coverage -Og -g>
                       $<$<CONFIG:Release>:
                            -O3
                            -DNDEBUG
                            -march=native
                            -mtune=native
                            -funroll-loops
                            -fomit-frame-pointer>)
