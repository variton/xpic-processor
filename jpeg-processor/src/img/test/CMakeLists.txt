
add_executable(test_img_hdr
               test_img_hdr.cpp)

target_include_directories(test_img_hdr PUBLIC
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/img>
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/core>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(test_img_hdr PRIVATE
                      img
                      doctest::doctest
                      $<$<CONFIG:Debug>:--coverage>)

target_compile_options(test_img_hdr PRIVATE
                       -Wall -Wextra -pedantic
                       $<$<CONFIG:Debug>:--coverage -Og -g>
                       $<$<CONFIG:Release>:-O3 -DNDEBUG>)

add_test(NAME test_img_hdr COMMAND $<TARGET_FILE:test_img_hdr>)

set_tests_properties(test_img_hdr PROPERTIES
  ENVIRONMENT "RC=${TRC};OUT=${TOUT}"
)

#=======================================================================

add_executable(test_jpeg_compressor
               test_jpeg_compressor.cpp)

target_include_directories(test_jpeg_compressor PUBLIC
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/img>
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/fio>
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/core>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(test_jpeg_compressor PRIVATE
                      img
                      fio
                      doctest::doctest
                      $<$<CONFIG:Debug>:--coverage>)

target_compile_options(test_jpeg_compressor PRIVATE
                       -Wall -Wextra -pedantic
                       $<$<CONFIG:Debug>:--coverage -Og -g>
                       $<$<CONFIG:Release>:-O3 -DNDEBUG>)

add_test(NAME test_jpeg_compressor COMMAND $<TARGET_FILE:test_jpeg_compressor>)

set_tests_properties(test_jpeg_compressor PROPERTIES
  ENVIRONMENT "RC=${TRC};OUT=${TOUT}"
)

#=======================================================================

add_executable(test_jpeg_decompressor
               test_jpeg_decompressor.cpp)

target_include_directories(test_jpeg_decompressor PUBLIC
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/img>
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/fio>
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/core>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(test_jpeg_decompressor PRIVATE
                      img
                      fio
                      doctest::doctest
                      $<$<CONFIG:Debug>:--coverage>)

target_compile_options(test_jpeg_decompressor PRIVATE
                       -Wall -Wextra -pedantic
                       $<$<CONFIG:Debug>:--coverage -Og -g>
                       $<$<CONFIG:Release>:-O3 -DNDEBUG>)

add_test(NAME test_jpeg_decompressor COMMAND $<TARGET_FILE:test_jpeg_decompressor>)

set_tests_properties(test_jpeg_decompressor PROPERTIES
  ENVIRONMENT "RC=${TRC};OUT=${TOUT}"
)

#=======================================================================

add_executable(test_blender
               test_blender.cpp)

target_include_directories(test_blender PUBLIC
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/img>
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/core>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(test_blender PRIVATE
                      img
                      fio
                      doctest::doctest
                      $<$<CONFIG:Debug>:--coverage>)

target_compile_options(test_blender PRIVATE
                       -Wall -Wextra -pedantic
                       $<$<CONFIG:Debug>:--coverage -Og -g>
                       $<$<CONFIG:Release>:-O3 -DNDEBUG>)

add_test(NAME test_blender COMMAND $<TARGET_FILE:test_blender>)

set_tests_properties(test_blender PROPERTIES
  ENVIRONMENT "RC=${TRC};OUT=${TOUT}"
)

#=======================================================================


