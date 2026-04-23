
add_library(fio SHARED
            filehandler.cpp)

target_include_directories(fio PUBLIC
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/>
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/core>
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/err>
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/topology>
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/fio>
  $<INSTALL_INTERFACE:include>
)

target_link_options(fio PRIVATE 
                    $<$<CONFIG:Debug>:--coverage>)

target_compile_options(fio PRIVATE 
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
