cmake_minimum_required (VERSION 3.7.0)

# set the project name
project (STEMulation++)

# include additional cmake scripts
include (scripts/clang-dev-tools.cmake)
include (scripts/generate-cmakelists.cmake)
include (scripts/iwyu.cmake)
include (ProcessorCount)

# get the name of the directory containing the parent CMakeList, if it exists
get_directory_property (parent_directory PARENT_DIRECTORY)

# set some CMAKE flags
set (CMAKE_VERBOSE_MAKEFILE ON)
set (CMAKE_COLOR_MAKEFILE ON)
set (CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set (CMAKE_DEBUG_POSTFIX "d")
#set (CMAKE_DISABLE_SOURCE_CHANGES ON)

# query the number of available processors for parallel building (on some compilers/platforms)
ProcessorCount (PROCESSOR_COUNT)

set (CMAKE_CXX_MP_FLAG ON CACHE BOOL "Parallel build enabled")
set (CMAKE_CXX_MP_NUM_PROCESSORS
     ${PROCESSOR_COUNT}
     CACHE STRING "The maximum number of processes for parallel build" FORCE)

if (DEFINED CMAKE_CXX_MP_NUM_PROCESSORS AND CMAKE_CXX_MP_FLAG)
    # parallel building for eclipse using gcc
    if (DEFINED CMAKE_ECLIPSE_MAKE_ARGUMENTS AND NOT "${CMAKE_ECLIPSE_MAKE_ARGUMENTS}" MATCHES "-j")
        set (CMAKE_ECLIPSE_MAKE_ARGUMENTS
             "${CMAKE_ECLIPSE_MAKE_ARGUMENTS} -j${CMAKE_CXX_MP_NUM_PROCESSORS}"
             CACHE STRING "Adding -j MAKE flag" FORCE)
    endif ()

    # enable /MP flag for Visual Studio 2008 and greater
    if (MSVC_VERSION GREATER 1400)
        foreach (outputconfig ${CMAKE_CONFIGURATION_TYPES})
            string (TOUPPER ${outputconfig} OUTPUTCONFIG)
            if (NOT "${CMAKE_CXX_FLAGS_${OUTPUTCONFIG}}" MATCHES ".*/MP.*")
                set (CMAKE_CXX_FLAGS_${OUTPUTCONFIG}
                     "${CMAKE_CXX_FLAGS_${OUTPUTCONFIG}} /MP${CMAKE_CXX_MP_NUM_PROCESSORS}"
                     CACHE STRING "Adding /MP CXX Flag" FORCE)
            endif ()
            if (NOT "${CMAKE_C_FLAGS_${OUTPUTCONFIG}}" MATCHES ".*/MP.*")
                set (CMAKE_C_FLAGS_${OUTPUTCONFIG}
                     "${CMAKE_C_FLAGS_${OUTPUTCONFIG}} /MP${CMAKE_CXX_MP_NUM_PROCESSORS}"
                     CACHE STRING "Adding /MP C Flag" FORCE)
            endif ()
        endforeach ()
    endif ()
endif ()

# disallow in-source builds
if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message (SEND_ERROR "In-source builds are not allowed.")
endif ()

# set the prefix for shared libraries
set (CMAKE_SHARED_LIBRARY_PREFIX "lib")

# set some install directories
if (NOT APPLE)
    set (ARCHIVE_INSTALL_DIR "lib/static")
    set (BINARY_INSTALL_DIR "bin")
    set (LIBRARY_INSTALL_DIR "lib")
else ()
    set (ARCHIVE_INSTALL_DIR ".")
    set (BINARY_INSTALL_DIR ".")
    set (LIBRARY_INSTALL_DIR ".")
endif ()

# for single-configuration builds
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${BINARY_INSTALL_DIR})
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${LIBRARY_INSTALL_DIR})
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${ARCHIVE_INSTALL_DIR})

# first for the generic no-config case (e.g. with mingw)
if (NOT CMAKE_CONFIGURATION_TYPES)
    # set the build type if binary build directory contains "debug" or "release"
    if ("${CMAKE_CURRENT_BINARY_DIR}" MATCHES ".*[Rr][Ee][Ll][Ee][Aa][Ss][Ee].*[Dd][Ee][Bb][Uu][Gg].*")
        set (CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "CMAKE_BUILD_TYPE: RelWithDebInfo" FORCE)
    elseif ("${CMAKE_CURRENT_BINARY_DIR}" MATCHES ".*[Dd][Ee][Bb][Uu][Gg].*")
        set (CMAKE_BUILD_TYPE Debug CACHE STRING "CMAKE_BUILD_TYPE: Debug" FORCE)
    elseif ("${CMAKE_CURRENT_BINARY_DIR}" MATCHES ".*[Rr][Ee][Ll][Ee][Aa][Ss][Ee].*")
        set (CMAKE_BUILD_TYPE Release CACHE STRING "CMAKE_BUILD_TYPE: Release" FORCE)
    endif ()
else ()
    # second, for multi-config builds (e.g. msvc)
    foreach (outputconfig ${CMAKE_CONFIGURATION_TYPES})
        string (TOUPPER ${outputconfig} OUTPUTCONFIG)
        set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/${BINARY_INSTALL_DIR})
        set (CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/${LIBRARY_INSTALL_DIR})
        set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/${ARCHIVE_INSTALL_DIR})
    endforeach ()
endif ()

# add additional sub-directories with more CMakeLists
add_subdirectory (third_party)
add_subdirectory (source)
add_subdirectory (unit_tests)

# add a target to perform source code formatting
if (clang_format_found)
    add_dev_tool("${EXE_CLANG_FORMAT}"
                 "${clang_format_arguments}"
                 "${sources}"
                 "${CMAKE_BINARY_DIR}/${clang_format_cfg}")
endif ()

# add a target to perform static analysis
if (clang_tidy_found)
    add_dev_tool("${EXE_CLANG_TIDY}"
                 "${clang_tidy_arguments}"
                 "${sources}"
                 "${CMAKE_BINARY_DIR}/${clang_tidy_cfg}")
endif ()

# include additional cmake scripts
include (scripts/testing.cmake)

# add a target to generate API documentation with Doxygen
find_package (Doxygen)
if (DOXYGEN_FOUND)
    string (REPLACE ";" " " doxygen_sources "${sources}")
    configure_file (${CMAKE_CURRENT_SOURCE_DIR}/scripts/doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/doxyfile @ONLY)
    add_custom_target (STEMulation++-doxygen
                       ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doxyfile
                       WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                       COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif ()

# perform the following only if the library has a parent scope
if (parent_directory)
    # relay list of sources to the parent scope
    if (parent_directory)
        set (stemulation_sources
             ${sources}
             ${unit_test_sources}
             PARENT_SCOPE)
    endif ()
endif ()

