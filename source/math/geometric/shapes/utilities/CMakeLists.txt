cmake_minimum_required (VERSION 3.7.0)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/polygon_triangulation_iterator.h
     ${CMAKE_CURRENT_LIST_DIR}/polygonTriangulator.cpp
     ${CMAKE_CURRENT_LIST_DIR}/polygonTriangulator.h
     PARENT_SCOPE)

