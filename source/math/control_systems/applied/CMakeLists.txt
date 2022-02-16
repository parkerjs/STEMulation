cmake_minimum_required (VERSION 3.7.0)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/bangBangServoMechanicalGimbal.cpp
     ${CMAKE_CURRENT_LIST_DIR}/bangBangServoMechanicalGimbal.h
     ${CMAKE_CURRENT_LIST_DIR}/secondOrderSystem.cpp
     ${CMAKE_CURRENT_LIST_DIR}/secondOrderSystem.h
     ${CMAKE_CURRENT_LIST_DIR}/servoMechanicalGimbal.cpp
     ${CMAKE_CURRENT_LIST_DIR}/servoMechanicalGimbal.h
     PARENT_SCOPE)

