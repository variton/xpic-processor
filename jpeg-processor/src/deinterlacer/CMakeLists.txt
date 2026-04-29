
add_executable(deinterlacer
    main.cpp
)

target_include_directories(deinterlacer PUBLIC
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/>
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/img>
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/utils>
  $<BUILD_INTERFACE:${CMAKE_INCLUDE_PATH}/fio>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(deinterlacer PRIVATE 
                      img
                      fio
                      utils
                      fmt::fmt)

target_link_options(deinterlacer PRIVATE 
                    $<$<CONFIG:Debug>:--coverage>)

target_compile_options(deinterlacer PRIVATE 
                       -Wall -Wextra -pedantic
                       $<$<CONFIG:Debug>:--coverage -Og -g>
                       $<$<CONFIG:Release>:
                            -O3 
                            -DNDEBUG
                            -march=native
                            -mtune=native
                            -funroll-loops
                            -fomit-frame-pointer>)
