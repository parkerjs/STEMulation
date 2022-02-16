cmake_minimum_required (VERSION 3.7.0)

# add additional sub-directories with more CMakeLists
add_subdirectory (combinatorics)
add_subdirectory (control_systems)
add_subdirectory (curves)
add_subdirectory (differentiators)
add_subdirectory (expression)
add_subdirectory (geometric)
add_subdirectory (integrators)
add_subdirectory (interpolators)
add_subdirectory (linear_algebra)
add_subdirectory (measurement)
add_subdirectory (number_systems)
add_subdirectory (sets)
add_subdirectory (signal_processing)
add_subdirectory (statistical)
add_subdirectory (trigonometric)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/math_constants.h
     PARENT_SCOPE)

