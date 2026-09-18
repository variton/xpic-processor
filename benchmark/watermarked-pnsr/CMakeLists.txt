
add_executable(sequence-pnsr
    sequence.cpp
)

target_include_directories(sequence-pnsr PUBLIC
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/quality>
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/img>
  $<BUILD_INTERFACE:${tcxx_SOURCE_DIR}/include/core>
  $<BUILD_INTERFACE:${tcxx_SOURCE_DIR}/include/obs>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(sequence-pnsr PRIVATE
                      img
                      quality 
                      fmt::fmt)

target_link_options(sequence-pnsr PRIVATE 
                    $<$<CONFIG:Debug>:--coverage>)

target_compile_options(sequence-pnsr PRIVATE 
                       -Wall -Wextra -pedantic
                       $<$<CONFIG:Debug>:--coverage -Og -g>
                       $<$<CONFIG:Release>:
                            -O3 
                            -DNDEBUG
                            -march=native
                            -mtune=native
                            -funroll-loops
                            -fomit-frame-pointer>)
