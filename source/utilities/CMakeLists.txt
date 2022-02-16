cmake_minimum_required (VERSION 3.7.0)

# add additional sub-directories with more CMakeLists
add_subdirectory (calendar)
add_subdirectory (expression)
add_subdirectory (file_system)
add_subdirectory (json)
add_subdirectory (string)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/algorithm.h
     ${CMAKE_CURRENT_LIST_DIR}/cli_argument_processor.h
     ${CMAKE_CURRENT_LIST_DIR}/csv_tokenizer.h
     ${CMAKE_CURRENT_LIST_DIR}/custom_locale.h
     ${CMAKE_CURRENT_LIST_DIR}/dictionary.h
     ${CMAKE_CURRENT_LIST_DIR}/logging_level.h
     ${CMAKE_CURRENT_LIST_DIR}/thread_pool.h
     ${CMAKE_CURRENT_LIST_DIR}/toggleable_stream.h
     ${CMAKE_CURRENT_LIST_DIR}/token_iterator.h
     ${CMAKE_CURRENT_LIST_DIR}/tokenizer.h
     ${CMAKE_CURRENT_LIST_DIR}/variable_registry.h
     ${CMAKE_CURRENT_LIST_DIR}/variable_registry_entry.h
     PARENT_SCOPE)

