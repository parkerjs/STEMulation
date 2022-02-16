cmake_minimum_required (VERSION 3.7.0)

# add additional sub-directories with more CMakeLists
add_subdirectory (detail)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/cat.hpp
     ${CMAKE_CURRENT_LIST_DIR}/elem.hpp
     ${CMAKE_CURRENT_LIST_DIR}/enum.hpp
     ${CMAKE_CURRENT_LIST_DIR}/first_n.hpp
     ${CMAKE_CURRENT_LIST_DIR}/fold_left.hpp
     ${CMAKE_CURRENT_LIST_DIR}/for_each_i.hpp
     ${CMAKE_CURRENT_LIST_DIR}/rest_n.hpp
     ${CMAKE_CURRENT_LIST_DIR}/seq.hpp
     ${CMAKE_CURRENT_LIST_DIR}/size.hpp
     ${CMAKE_CURRENT_LIST_DIR}/subseq.hpp
     ${CMAKE_CURRENT_LIST_DIR}/transform.hpp
     PARENT_SCOPE)

