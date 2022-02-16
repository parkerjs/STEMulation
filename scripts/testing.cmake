cmake_minimum_required (VERSION 3.7.0)

# set the unit tester executable
set (unit_tester_executable "${PROJECT_NAME}UnitTester")

# add unit tests
foreach (source ${sources})
    if ("${source}" MATCHES ".*test.*\..cpp")
        get_filename_component (test "${source}" NAME_WE)
        if ("${test}" MATCHES "^test.*")
            add_test (NAME "${test}"
                      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                      COMMAND ${unit_tester_executable} "-test" "${test}")
        endif ()
    endif ()
endforeach ()

# make a directory for binary inputs and outputs
file (MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/bin/inputs")
file (MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/bin/outputs")

# copy inputs to destination for unit-testing
file (COPY "${CMAKE_CURRENT_SOURCE_DIR}/inputs" DESTINATION "${CMAKE_BINARY_DIR}/bin")

# add a target to build the executable and run unit tests
add_custom_target ("RUN_${PROJECT_NAME}_UNIT_TESTS"
                   COMMAND ${CMAKE_COMMAND} --build . --target "${unit_tester_executable}" --config $<CONFIGURATION>
                   COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/inputs/
                   COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/outputs/
                   COMMAND ${CMAKE_COMMAND} -E echo CWD=${CMAKE_BINARY_DIR}
                   COMMAND ${CMAKE_COMMAND} -E echo CMD=${CMAKE_CTEST_COMMAND} -C $<CONFIG>
                   COMMAND ${CMAKE_COMMAND} -E echo ----------------------------------
                   COMMAND ${CMAKE_COMMAND} -E env CTEST_OUTPUT_ON_FAILURE=1
                   ${CMAKE_CTEST_COMMAND} "-C" "$<CONFIG>" "-R" "test.+"
#                  ${CMAKE_CTEST_COMMAND} "-C" "$<CONFIG>" --show-only
                   WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                   DEPENDS ${unit_tester_executable}
                           ${PROJECT_NAME})
