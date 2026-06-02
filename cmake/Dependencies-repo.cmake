
# target_include_directories(xpic_processor PRIVATE
#     ${tcxx_SOURCE_DIR}/include
# )

include(FetchContent)

set(FETCHCONTENT_BASE_DIR
    "${CMAKE_SOURCE_DIR}/.deps"
    CACHE PATH "FetchContent cache"
    FORCE
)

#tcxx-20 ========================================================

FetchContent_Declare(
    tcxx
    GIT_REPOSITORY https://github.com/variton/tcxx.git
    GIT_TAG main
    GIT_SHALLOW TRUE

    # Prevent tcxx's own CMakeLists.txt from being included
    SOURCE_SUBDIR cmake-do-not-add
)

FetchContent_MakeAvailable(tcxx)

#doctest =======================================================

set(DOCTEST_SOURCE_DIR "${FETCHCONTENT_BASE_DIR}/doctest-src")

if(EXISTS "${DOCTEST_SOURCE_DIR}/CMakeLists.txt")
    message(STATUS "Using cached doctest: ${DOCTEST_SOURCE_DIR}")

    set(FETCHCONTENT_SOURCE_DIR_DOCTEST
        "${DOCTEST_SOURCE_DIR}"
        CACHE PATH "Cached doctest source"
        FORCE
    )
else()
    message(STATUS "Fetching doctest")
endif()

FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG v2.4.12
)

FetchContent_MakeAvailable(doctest)
