cmake_minimum_required (VERSION 3.7.0)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/any_configurable.h
     ${CMAKE_CURRENT_LIST_DIR}/authenticatable.h
     ${CMAKE_CURRENT_LIST_DIR}/cli_argument_processable.h
     ${CMAKE_CURRENT_LIST_DIR}/cli_configurable.h
     ${CMAKE_CURRENT_LIST_DIR}/cli_usage_menu_printable.h
     ${CMAKE_CURRENT_LIST_DIR}/cloneable.h
     ${CMAKE_CURRENT_LIST_DIR}/configurable.h
     ${CMAKE_CURRENT_LIST_DIR}/date_formattable.h
     ${CMAKE_CURRENT_LIST_DIR}/executable.h
     ${CMAKE_CURRENT_LIST_DIR}/file_configurable.h
     ${CMAKE_CURRENT_LIST_DIR}/finalizable.h
     ${CMAKE_CURRENT_LIST_DIR}/indexable.h
     ${CMAKE_CURRENT_LIST_DIR}/initializable.h
     ${CMAKE_CURRENT_LIST_DIR}/nameable.h
     ${CMAKE_CURRENT_LIST_DIR}/publisher_subscribable.h
     ${CMAKE_CURRENT_LIST_DIR}/registry_configurable.h
     ${CMAKE_CURRENT_LIST_DIR}/serializable.h
     ${CMAKE_CURRENT_LIST_DIR}/setupable.h
     ${CMAKE_CURRENT_LIST_DIR}/swappable.h
     ${CMAKE_CURRENT_LIST_DIR}/token_map_configurable.h
     ${CMAKE_CURRENT_LIST_DIR}/updateable.h
     ${CMAKE_CURRENT_LIST_DIR}/xml_readable.h
     ${CMAKE_CURRENT_LIST_DIR}/xml_writable.h
     PARENT_SCOPE)

