# cmake/Doxygen.cmake
# Usage:
#   include(cmake/Doxygen.cmake)
#   (optional) set(DOXYGEN_INPUT_DIRS ...) etc BEFORE including.

set(DOXYGEN_OUTPUT_DIR "${PROJECT_SOURCE_DIR}/docs")

include_guard(GLOBAL)

option(BUILD_DOCS "Build documentation (Doxygen)" ON)

if (NOT BUILD_DOCS)
  return()
endif()

find_package(Doxygen QUIET)
if (NOT DOXYGEN_FOUND)
  message(STATUS "Doxygen not found: docs target disabled")
  return()
endif()

# ---- Defaults (override these before include() if you want) ----
if (NOT DEFINED DOXYGEN_OUTPUT_DIR)
  set(DOXYGEN_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/docs")
endif()

if (NOT DEFINED DOXYGEN_INPUT_DIRS)
  set(DOXYGEN_INPUT_DIRS
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_SOURCE_DIR}/src"
    "${PROJECT_SOURCE_DIR}/docs"
  )
endif()

if (NOT DEFINED DOXYGEN_MAINPAGE)
  if (EXISTS "${PROJECT_SOURCE_DIR}/README.md")
    set(DOXYGEN_MAINPAGE "${PROJECT_SOURCE_DIR}/README.md")
  else()
    set(DOXYGEN_MAINPAGE "")
  endif()
endif()

# Convert list to space-separated for Doxygen
set(_doxygen_inputs "${DOXYGEN_INPUT_DIRS}")
string(REPLACE ";" " " DOXYGEN_INPUT_DIRS "${_doxygen_inputs}")

# Optional Graphviz (dot)
if (NOT DEFINED DOXYGEN_HAVE_DOT)
  find_program(DOT_EXECUTABLE dot)
  if (DOT_EXECUTABLE)
    set(DOXYGEN_HAVE_DOT "YES")
  else()
    set(DOXYGEN_HAVE_DOT "NO")
  endif()
endif()

# Where the template lives (default: docs/Doxyfile.in in your source tree)
if (NOT DEFINED DOXYFILE_IN)
  set(DOXYFILE_IN "${PROJECT_SOURCE_DIR}/docs/Doxyfile.in")
endif()

if (NOT EXISTS "${DOXYFILE_IN}")
  message(FATAL_ERROR "Doxygen template not found: ${DOXYFILE_IN}")
endif()

set(DOXYFILE_OUT "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile")
configure_file("${DOXYFILE_IN}" "${DOXYFILE_OUT}" @ONLY)

add_custom_target(doxygen
  COMMAND "${DOXYGEN_EXECUTABLE}" "${DOXYFILE_OUT}"
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
  COMMENT "Generating API documentation with Doxygen"
  VERBATIM
)

# Nice-to-have: make it show up in IDEs
set_target_properties(doxygen PROPERTIES FOLDER "docs")

message(STATUS "Docs target enabled: doxygen (output: ${DOXYGEN_OUTPUT_DIR}/html)")
