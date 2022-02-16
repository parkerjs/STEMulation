cmake_minimum_required (VERSION 3.7.0)

# add additional sub-directories with more CMakeLists
add_subdirectory (operators)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/has_arithmetic_precision_loss.h
     ${CMAKE_CURRENT_LIST_DIR}/has_member_function.h
     ${CMAKE_CURRENT_LIST_DIR}/has_member_type.h
     ${CMAKE_CURRENT_LIST_DIR}/is_abstract.h
     ${CMAKE_CURRENT_LIST_DIR}/is_associative_container.h
     ${CMAKE_CURRENT_LIST_DIR}/is_complex.h
     ${CMAKE_CURRENT_LIST_DIR}/is_container.h
     ${CMAKE_CURRENT_LIST_DIR}/is_derived_from_template.h
     ${CMAKE_CURRENT_LIST_DIR}/is_function_ptr.h
     ${CMAKE_CURRENT_LIST_DIR}/is_input_streamable.h
     ${CMAKE_CURRENT_LIST_DIR}/is_output_streamable.h
     ${CMAKE_CURRENT_LIST_DIR}/is_pair_associative_container.h
     ${CMAKE_CURRENT_LIST_DIR}/is_reference_wrapped.h
     ${CMAKE_CURRENT_LIST_DIR}/is_resizeable_sequence_container.h
     ${CMAKE_CURRENT_LIST_DIR}/is_reverse_iterable.h
     ${CMAKE_CURRENT_LIST_DIR}/is_sequence_container.h
     ${CMAKE_CURRENT_LIST_DIR}/is_specialization_of.h
     ${CMAKE_CURRENT_LIST_DIR}/is_std_basic_string.h
     ${CMAKE_CURRENT_LIST_DIR}/is_std_set.h
     ${CMAKE_CURRENT_LIST_DIR}/is_std_vector.h
     ${CMAKE_CURRENT_LIST_DIR}/tuple_has_type.h
     PARENT_SCOPE)

