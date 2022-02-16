cmake_minimum_required (VERSION 3.7.0)

# add additional sub-directories with more CMakeLists
add_subdirectory (detail)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/deduce_r.hpp
     ${CMAKE_CURRENT_LIST_DIR}/deduce_z.hpp
     ${CMAKE_CURRENT_LIST_DIR}/enum.hpp
     ${CMAKE_CURRENT_LIST_DIR}/enum_binary_params.hpp
     ${CMAKE_CURRENT_LIST_DIR}/enum_params.hpp
     ${CMAKE_CURRENT_LIST_DIR}/enum_params_with_a_default.hpp
     ${CMAKE_CURRENT_LIST_DIR}/enum_params_with_defaults.hpp
     ${CMAKE_CURRENT_LIST_DIR}/enum_shifted.hpp
     ${CMAKE_CURRENT_LIST_DIR}/enum_shifted_binary_params.hpp
     ${CMAKE_CURRENT_LIST_DIR}/enum_shifted_params.hpp
     ${CMAKE_CURRENT_LIST_DIR}/enum_trailing.hpp
     ${CMAKE_CURRENT_LIST_DIR}/enum_trailing_binary_params.hpp
     ${CMAKE_CURRENT_LIST_DIR}/enum_trailing_params.hpp
     ${CMAKE_CURRENT_LIST_DIR}/for.hpp
     ${CMAKE_CURRENT_LIST_DIR}/repeat.hpp
     ${CMAKE_CURRENT_LIST_DIR}/repeat_from_to.hpp
     PARENT_SCOPE)

