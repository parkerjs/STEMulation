cmake_minimum_required (VERSION 3.7.0)

# add additional sub-directories with more CMakeLists
add_subdirectory (orientation)
add_subdirectory (shapes)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/angularSubtense.cpp
     ${CMAKE_CURRENT_LIST_DIR}/angularSubtense.h
     ${CMAKE_CURRENT_LIST_DIR}/line.cpp
     ${CMAKE_CURRENT_LIST_DIR}/line.h
     ${CMAKE_CURRENT_LIST_DIR}/plane.cpp
     ${CMAKE_CURRENT_LIST_DIR}/plane.h
     ${CMAKE_CURRENT_LIST_DIR}/ray.cpp
     ${CMAKE_CURRENT_LIST_DIR}/ray.h
     ${CMAKE_CURRENT_LIST_DIR}/rayIntersection.cpp
     ${CMAKE_CURRENT_LIST_DIR}/rayIntersection.h
     PARENT_SCOPE)

