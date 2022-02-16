cmake_minimum_required (VERSION 3.7.0)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/deadband.h
     ${CMAKE_CURRENT_LIST_DIR}/limiter.h
     ${CMAKE_CURRENT_LIST_DIR}/nonlinearity.h
     ${CMAKE_CURRENT_LIST_DIR}/phase_unwrapper.h
     PARENT_SCOPE)

