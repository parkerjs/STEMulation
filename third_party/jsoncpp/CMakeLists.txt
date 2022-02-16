cmake_minimum_required (VERSION 3.7.0)

# set the project name
project (JsonCpp)

# set target for this build
set (TARGET ${PROJECT_NAME})

# first for the generic no-config case (e.g. with mingw)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# second, for multi-config builds (e.g. msvc)
foreach (outputconfig ${CMAKE_CONFIGURATION_TYPES})
    string (TOUPPER ${outputconfig} OUTPUTCONFIG)
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_${outputconfig} ${CMAKE_BINARY_DIR}/bin)
    set (CMAKE_LIBRARY_OUTPUT_DIRECTORY_${outputconfig} ${CMAKE_BINARY_DIR}/bin)
    set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${outputconfig} ${CMAKE_BINARY_DIR}/bin)
endforeach ()

# set JsonCpp_FOUND, JsonCpp_INCLUDE_DIR, JsonCpp_DIR for GUI
set (JsonCpp_FOUND
     TRUE
     PARENT_SCOPE)
set (JsonCpp_INCLUDE_DIR
     ${CMAKE_CURRENT_LIST_DIR}
     PARENT_SCOPE)
set (JsonCpp_DIR
     "${CMAKE_CURRENT_LIST_DIR}"
     CACHE PATH "" FORCE)

# add sources to the project
set (sources
     ${CMAKE_CURRENT_LIST_DIR}/json-forwards.h
     ${CMAKE_CURRENT_LIST_DIR}/json.h
     ${CMAKE_CURRENT_LIST_DIR}/jsoncpp.cpp)

# sort the list of sources
list (SORT sources)

# add a static library
add_library (${TARGET} STATIC ${sources})

# set target properties
set_target_properties (${TARGET}
                       PROPERTIES BUILD_WITH_INSTALL_RPATH 1
                                  CXX_STANDARD 14
                                  DEBUG_POSTFIX "d"
                                  DEFINE_SYMBOL "JSON_IS_AMALGAMATION"
                                  INSTALL_NAME_DIR "@rpath")

# if compiling on windows, make sure to define UNICODE
if (WIN32)
    target_compile_definitions (${TARGET} PRIVATE
                                UNICODE)
endif ()

# set platform-specific compiler flags
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
    # using Clang
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # using GCC
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    # using Intel C++
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # using Visual Studio C++
    target_compile_definitions (${TARGET} PRIVATE
                                _CRT_SECURE_NO_WARNINGS
                                _SCL_SECURE_NO_WARNINGS
                                NOMINMAX)
endif ()

# set installation directory
install (TARGETS ${TARGET}
         DESTINATION ${CMAKE_BINARY_DIR}/bin)
