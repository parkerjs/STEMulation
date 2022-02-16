cmake_minimum_required (VERSION 3.7.0)

# set the project name
project (STEMulation++UnitTester)

# set target for this build
set (TARGET ${PROJECT_NAME})

# add sources to the project
set (unit_test_sources
     ${CMAKE_CURRENT_LIST_DIR}/testBalancedExpressionChecker.cpp
     ${CMAKE_CURRENT_LIST_DIR}/testBalancedExpressionChecker.h
     ${CMAKE_CURRENT_LIST_DIR}/testBezier.cpp
     ${CMAKE_CURRENT_LIST_DIR}/testBezier.h
     ${CMAKE_CURRENT_LIST_DIR}/testCholesky.cpp
     ${CMAKE_CURRENT_LIST_DIR}/testCholesky.h
     ${CMAKE_CURRENT_LIST_DIR}/testComplexMatrix2d.cpp
     ${CMAKE_CURRENT_LIST_DIR}/testComplexMatrix2d.h
     ${CMAKE_CURRENT_LIST_DIR}/testComplexMatrixNd.cpp
     ${CMAKE_CURRENT_LIST_DIR}/testComplexMatrixNd.h
     ${CMAKE_CURRENT_LIST_DIR}/testCroutLU.cpp
     ${CMAKE_CURRENT_LIST_DIR}/testCroutLU.h
     ${CMAKE_CURRENT_LIST_DIR}/testDate.cpp
     ${CMAKE_CURRENT_LIST_DIR}/testDate.h
     ${CMAKE_CURRENT_LIST_DIR}/testDependencyInjectable.cpp
     ${CMAKE_CURRENT_LIST_DIR}/testDependencyInjectable.h
     ${CMAKE_CURRENT_LIST_DIR}/testDirectoryTraverser.cpp
     ${CMAKE_CURRENT_LIST_DIR}/testDirectoryTraverser.h
     ${CMAKE_CURRENT_LIST_DIR}/testDoolittleLU.cpp
     ${CMAKE_CURRENT_LIST_DIR}/testDoolittleLU.h
     ${CMAKE_CURRENT_LIST_DIR}/testExpressionTree.cpp
     ${CMAKE_CURRENT_LIST_DIR}/testExpressionTree.h
     ${CMAKE_CURRENT_LIST_DIR}/testGeneralMatrix2d.cpp
     ${CMAKE_CURRENT_LIST_DIR}/testGeneralMatrix2d.h
     ${CMAKE_CURRENT_LIST_DIR}/testGeneralMatrixNd.cpp
     ${CMAKE_CURRENT_LIST_DIR}/testGeneralMatrixNd.h
     ${CMAKE_CURRENT_LIST_DIR}/testNumericMatrix2d.cpp
     ${CMAKE_CURRENT_LIST_DIR}/testNumericMatrix2d.h
     ${CMAKE_CURRENT_LIST_DIR}/testNumericMatrixNd.cpp
     ${CMAKE_CURRENT_LIST_DIR}/testNumericMatrixNd.h
     ${CMAKE_CURRENT_LIST_DIR}/testPolynomial.cpp
     ${CMAKE_CURRENT_LIST_DIR}/testPolynomial.h
     ${CMAKE_CURRENT_LIST_DIR}/testPrefixTree.cpp
     ${CMAKE_CURRENT_LIST_DIR}/testPrefixTree.h
     ${CMAKE_CURRENT_LIST_DIR}/testPublisherSubscriber.cpp
     ${CMAKE_CURRENT_LIST_DIR}/testPublisherSubscriber.h
     ${CMAKE_CURRENT_LIST_DIR}/testQR.cpp
     ${CMAKE_CURRENT_LIST_DIR}/testQR.h
     ${CMAKE_CURRENT_LIST_DIR}/testRealMatrix2d.cpp
     ${CMAKE_CURRENT_LIST_DIR}/testRealMatrix2d.h
     ${CMAKE_CURRENT_LIST_DIR}/testRealMatrixNd.cpp
     ${CMAKE_CURRENT_LIST_DIR}/testRealMatrixNd.h
     ${CMAKE_CURRENT_LIST_DIR}/testSequenceConvolution.cpp
     ${CMAKE_CURRENT_LIST_DIR}/testSequenceConvolution.h
     ${CMAKE_CURRENT_LIST_DIR}/testStatistical.cpp
     ${CMAKE_CURRENT_LIST_DIR}/testStatistical.h
     ${CMAKE_CURRENT_LIST_DIR}/testStringUtilities.cpp
     ${CMAKE_CURRENT_LIST_DIR}/testStringUtilities.h
     ${CMAKE_CURRENT_LIST_DIR}/testSubscript.cpp
     ${CMAKE_CURRENT_LIST_DIR}/testSubscript.h
     ${CMAKE_CURRENT_LIST_DIR}/testTridiagonalLU.cpp
     ${CMAKE_CURRENT_LIST_DIR}/testTridiagonalLU.h
     ${CMAKE_CURRENT_LIST_DIR}/testURL_Parser.cpp
     ${CMAKE_CURRENT_LIST_DIR}/testURL_Parser.h
     ${CMAKE_CURRENT_LIST_DIR}/testVariableWrapper.cpp
     ${CMAKE_CURRENT_LIST_DIR}/testVariableWrapper.h
     ${CMAKE_CURRENT_LIST_DIR}/unitTest.cpp
     ${CMAKE_CURRENT_LIST_DIR}/unitTest.h
     ${CMAKE_CURRENT_LIST_DIR}/unitTestManager.cpp
     ${CMAKE_CURRENT_LIST_DIR}/unitTestManager.h)

# sort the list of sources
list (SORT unit_test_sources)

# get the absolute path of the parent folder
get_filename_component (parent_absolute_path ${CMAKE_CURRENT_SOURCE_DIR}/../ ABSOLUTE)

# determine include paths, define source groups
foreach (source IN LISTS sources)
    get_filename_component (group "${source}" PATH)
    string (REPLACE ${parent_absolute_path} ".." group ${group})
    set (include_paths
         ${include_paths} ${group})
endforeach ()

# add additional include paths
set (include_paths
     ${include_paths}
     ${third_party_library_includes})

# add source group
source_group (unit_tests
              FILES ${CMAKE_CURRENT_LIST_DIR}/test.cpp
              ${unit_test_sources})

# remove duplicates from include directories
list (REMOVE_DUPLICATES include_paths)

# sort the list of include directories
list (SORT include_paths)

# add include directories
include_directories (${include_paths})

# add an executable to this project
add_executable (${TARGET} ${CMAKE_CURRENT_LIST_DIR}/test.cpp ${unit_test_sources})

# libraries to which binary will be linked
target_link_libraries (${TARGET} "STEMulation++")

# set the rpath to be relative to the path from which the library was loaded
set_target_properties (${TARGET}
                       PROPERTIES INSTALL_RPATH "@loader_path/"
                                  CXX_STANDARD 17)

# if compiling on windows, make sure to define UNICODE
if (WIN32)
    target_compile_definitions(${TARGET} PRIVATE
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

# add a "d" to the end of binaries for debug builds
set_target_properties (${TARGET}
                       PROPERTIES DEBUG_POSTFIX "d")

# set installation directory
install (TARGETS ${TARGET} DESTINATION ${CMAKE_BINARY_DIR}/bin)

# relay list of sources to the parent scope
set (sources
     ${sources}
     ${unit_test_sources}
     PARENT_SCOPE)
