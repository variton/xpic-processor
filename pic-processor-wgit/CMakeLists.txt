cmake_minimum_required(VERSION 3.31.6)
project(pic-processor)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_INIT "-Wall -Wno-overflow -pendantic -std=c++20")

set(CMAKE_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/include)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(Dependencies)

SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
SET(TRC ${CMAKE_CURRENT_SOURCE_DIR}/rc)
SET(TOUT ${CMAKE_CURRENT_SOURCE_DIR}/test-results)

file(GLOB SOURCES "src/*.cpp")

enable_testing()
#source code
add_subdirectory(src)
add_subdirectory(test)
#non regression test
#add_subdirectory(nrt)
#tool for coverage
include(Coverage)
#tool for documentation
include(Doxygen)
