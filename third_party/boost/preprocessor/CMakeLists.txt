cmake_minimum_required (VERSION 3.7.0)

# add additional sub-directories with more CMakeLists
add_subdirectory (arithmetic)
add_subdirectory (array)
add_subdirectory (comparison)
add_subdirectory (config)
add_subdirectory (control)
add_subdirectory (debug)
add_subdirectory (detail)
add_subdirectory (facilities)
add_subdirectory (iteration)
add_subdirectory (list)
add_subdirectory (logical)
add_subdirectory (punctuation)
add_subdirectory (repetition)
add_subdirectory (seq)
add_subdirectory (slot)
add_subdirectory (tuple)
add_subdirectory (variadic)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/cat.hpp
     ${CMAKE_CURRENT_LIST_DIR}/comma_if.hpp
     ${CMAKE_CURRENT_LIST_DIR}/dec.hpp
     ${CMAKE_CURRENT_LIST_DIR}/empty.hpp
     ${CMAKE_CURRENT_LIST_DIR}/enum.hpp
     ${CMAKE_CURRENT_LIST_DIR}/enum_params.hpp
     ${CMAKE_CURRENT_LIST_DIR}/enum_params_with_a_default.hpp
     ${CMAKE_CURRENT_LIST_DIR}/enum_params_with_defaults.hpp
     ${CMAKE_CURRENT_LIST_DIR}/enum_shifted_params.hpp
     ${CMAKE_CURRENT_LIST_DIR}/expr_if.hpp
     ${CMAKE_CURRENT_LIST_DIR}/identity.hpp
     ${CMAKE_CURRENT_LIST_DIR}/inc.hpp
     ${CMAKE_CURRENT_LIST_DIR}/iterate.hpp
     ${CMAKE_CURRENT_LIST_DIR}/repeat.hpp
     ${CMAKE_CURRENT_LIST_DIR}/repeat_from_to.hpp
     ${CMAKE_CURRENT_LIST_DIR}/repetition.hpp
     ${CMAKE_CURRENT_LIST_DIR}/stringize.hpp
     PARENT_SCOPE)

