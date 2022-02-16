cmake_minimum_required (VERSION 3.7.0)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/branch_counter.h
     ${CMAKE_CURRENT_LIST_DIR}/branch_printer.h
     ${CMAKE_CURRENT_LIST_DIR}/node_counter.h
     ${CMAKE_CURRENT_LIST_DIR}/node_printer.h
     ${CMAKE_CURRENT_LIST_DIR}/prefix_tree.h
     ${CMAKE_CURRENT_LIST_DIR}/tree_node.h
     PARENT_SCOPE)

