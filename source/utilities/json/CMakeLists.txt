cmake_minimum_required (VERSION 3.7.0)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/json_node_processor.h
     ${CMAKE_CURRENT_LIST_DIR}/json_to_csv_converter.h
     ${CMAKE_CURRENT_LIST_DIR}/json_to_prefix_tree_converter.h
     ${CMAKE_CURRENT_LIST_DIR}/json_to_table_converter.h
     PARENT_SCOPE)

