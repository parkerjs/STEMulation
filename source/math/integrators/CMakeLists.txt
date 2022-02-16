cmake_minimum_required (VERSION 3.7.0)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/adaptiveRungeKutta.cpp
     ${CMAKE_CURRENT_LIST_DIR}/adaptiveRungeKutta.h
     ${CMAKE_CURRENT_LIST_DIR}/butcherTableau.cpp
     ${CMAKE_CURRENT_LIST_DIR}/butcherTableau.h
     ${CMAKE_CURRENT_LIST_DIR}/rungeKutta.cpp
     ${CMAKE_CURRENT_LIST_DIR}/rungeKutta.h
     ${CMAKE_CURRENT_LIST_DIR}/rungeKutta4.cpp
     ${CMAKE_CURRENT_LIST_DIR}/rungeKutta4.h
     ${CMAKE_CURRENT_LIST_DIR}/rungeKuttaFehlberg45.cpp
     ${CMAKE_CURRENT_LIST_DIR}/rungeKuttaFehlberg45.h
     PARENT_SCOPE)

