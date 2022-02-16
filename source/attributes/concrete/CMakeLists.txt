cmake_minimum_required (VERSION 3.7.0)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/asynchronous.h
     ${CMAKE_CURRENT_LIST_DIR}/composite.h
     ${CMAKE_CURRENT_LIST_DIR}/date_configurable.h
     ${CMAKE_CURRENT_LIST_DIR}/dependency_injectable.h
     ${CMAKE_CURRENT_LIST_DIR}/future_mappable.h
     ${CMAKE_CURRENT_LIST_DIR}/input_streamable.h
     ${CMAKE_CURRENT_LIST_DIR}/loggable.h
     ${CMAKE_CURRENT_LIST_DIR}/logging_file_streamable.h
     ${CMAKE_CURRENT_LIST_DIR}/logging_streamable.h
     ${CMAKE_CURRENT_LIST_DIR}/mutex_mappable.h
     ${CMAKE_CURRENT_LIST_DIR}/output_streamable.h
     ${CMAKE_CURRENT_LIST_DIR}/prototype_constructible.h
     ${CMAKE_CURRENT_LIST_DIR}/static_asynchronous.h
     ${CMAKE_CURRENT_LIST_DIR}/static_authenticatable.h
     ${CMAKE_CURRENT_LIST_DIR}/static_cli_argument_processable.h
     ${CMAKE_CURRENT_LIST_DIR}/static_cli_configurable.h
     ${CMAKE_CURRENT_LIST_DIR}/static_dependency_injectable.h
     ${CMAKE_CURRENT_LIST_DIR}/static_file_configurable.h
     ${CMAKE_CURRENT_LIST_DIR}/static_future_mappable.h
     ${CMAKE_CURRENT_LIST_DIR}/static_initializable.h
     ${CMAKE_CURRENT_LIST_DIR}/static_loggable.h
     ${CMAKE_CURRENT_LIST_DIR}/static_logging_streamable.h
     ${CMAKE_CURRENT_LIST_DIR}/static_mutex_mappable.h
     ${CMAKE_CURRENT_LIST_DIR}/static_setupable.h
     ${CMAKE_CURRENT_LIST_DIR}/static_synchronizable.h
     ${CMAKE_CURRENT_LIST_DIR}/static_toggleable_streamable.h
     ${CMAKE_CURRENT_LIST_DIR}/static_token_map_configurable.h
     ${CMAKE_CURRENT_LIST_DIR}/static_variable_map_registrable.h
     ${CMAKE_CURRENT_LIST_DIR}/streamable.h
     ${CMAKE_CURRENT_LIST_DIR}/synchronizable.h
     ${CMAKE_CURRENT_LIST_DIR}/toggleable_streamable.h
     ${CMAKE_CURRENT_LIST_DIR}/tuple_attribute.h
     ${CMAKE_CURRENT_LIST_DIR}/variable_map_registrable.h
     ${CMAKE_CURRENT_LIST_DIR}/xml_constructible.h
     PARENT_SCOPE)

