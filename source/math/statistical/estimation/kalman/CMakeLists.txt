cmake_minimum_required (VERSION 3.7.0)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/extendedKalman.cpp
     ${CMAKE_CURRENT_LIST_DIR}/extendedKalman.h
     ${CMAKE_CURRENT_LIST_DIR}/kalman.cpp
     ${CMAKE_CURRENT_LIST_DIR}/kalman.h
     ${CMAKE_CURRENT_LIST_DIR}/linearKalman.cpp
     ${CMAKE_CURRENT_LIST_DIR}/linearKalman.h
     ${CMAKE_CURRENT_LIST_DIR}/unscentedKalman.cpp
     ${CMAKE_CURRENT_LIST_DIR}/unscentedKalman.h
     PARENT_SCOPE)

