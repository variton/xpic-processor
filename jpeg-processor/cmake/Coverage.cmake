if(LCOV_EXECUTABLE AND GENHTML_EXECUTABLE)
    add_custom_target(coverage
        # Reset previous counters in the build tree
        COMMAND ${LCOV_EXECUTABLE}
        --directory ${CMAKE_BINARY_DIR}
        --zerocounters

        # Run tests to generate fresh .gcda files
        COMMAND ${CMAKE_CTEST_COMMAND}
        --output-on-failure

        # Capture everything from the build tree first
        COMMAND ${LCOV_EXECUTABLE}
        --gcov-tool ${GCOV_EXECUTABLE}
        --capture
        --directory ${CMAKE_BINARY_DIR}
        --base-directory ${CMAKE_SOURCE_DIR}
        --ignore-errors inconsistent
        --output-file coverage.raw.info

        # Keep only your product code
        COMMAND ${LCOV_EXECUTABLE}
        --extract coverage.raw.info
        "${CMAKE_SOURCE_DIR}/src/*"
        "${CMAKE_SOURCE_DIR}/test/*"
        --output-file coverage.info

        # Optional: remove a few leftovers if your tree has vendored code under src/
        COMMAND ${LCOV_EXECUTABLE}
        --remove coverage.info
        #'*/tests/*'
        '*/external/*'
        '*/_deps/*'
        '*/doctest/*'
        --ignore-errors unused
        --output-file coverage.filtered.info

        # Generate HTML report
        COMMAND ${GENHTML_EXECUTABLE}
        coverage.filtered.info
        --output-directory coverage_html

        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running tests and generating lcov report for project sources only"
        VERBATIM
        )
endif()
