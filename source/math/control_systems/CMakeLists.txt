cmake_minimum_required (VERSION 3.7.0)

# add additional sub-directories with more CMakeLists
add_subdirectory (applied)
add_subdirectory (blocks)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/stateMap.cpp
     ${CMAKE_CURRENT_LIST_DIR}/stateMap.h
     ${CMAKE_CURRENT_LIST_DIR}/stateSpaceModel.cpp
     ${CMAKE_CURRENT_LIST_DIR}/stateSpaceModel.h
     ${CMAKE_CURRENT_LIST_DIR}/stateVector.cpp
     ${CMAKE_CURRENT_LIST_DIR}/stateVector.h
     PARENT_SCOPE)

