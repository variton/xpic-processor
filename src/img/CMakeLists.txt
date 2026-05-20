
add_library(img SHARED
            img_hdr.cpp
            jpeg_compressor.cpp
            jpeg_decompressor.cpp
            blender.cpp)

target_include_directories(img PUBLIC
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/img>
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/fio>
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/err>
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/topology>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(img PRIVATE 
                      fio
                      JPEG::JPEG)

target_link_options(img PRIVATE 
                    $<$<CONFIG:Debug>:--coverage>)

target_compile_options(utils PRIVATE 
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
