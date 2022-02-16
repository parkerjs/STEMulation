cmake_minimum_required (VERSION 3.7.0)

# add additional sub-directories with more CMakeLists
add_subdirectory (utilities)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/polygon.cpp
     ${CMAKE_CURRENT_LIST_DIR}/polygon.h
     ${CMAKE_CURRENT_LIST_DIR}/polygonMesh.cpp
     ${CMAKE_CURRENT_LIST_DIR}/polygonMesh.h
     ${CMAKE_CURRENT_LIST_DIR}/shape.cpp
     ${CMAKE_CURRENT_LIST_DIR}/shape.h
     ${CMAKE_CURRENT_LIST_DIR}/shape2d.cpp
     ${CMAKE_CURRENT_LIST_DIR}/shape2d.h
     ${CMAKE_CURRENT_LIST_DIR}/shape3d.h
     ${CMAKE_CURRENT_LIST_DIR}/sphere.cpp
     ${CMAKE_CURRENT_LIST_DIR}/sphere.h
     ${CMAKE_CURRENT_LIST_DIR}/triangle.cpp
     ${CMAKE_CURRENT_LIST_DIR}/triangle.h
     PARENT_SCOPE)

