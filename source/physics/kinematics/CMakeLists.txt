cmake_minimum_required (VERSION 3.7.0)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/acceleration_axis_type.h
     ${CMAKE_CURRENT_LIST_DIR}/cartesianMotionState.cpp
     ${CMAKE_CURRENT_LIST_DIR}/cartesianMotionState.h
     ${CMAKE_CURRENT_LIST_DIR}/coordinateSystem.cpp
     ${CMAKE_CURRENT_LIST_DIR}/coordinateSystem.h
     ${CMAKE_CURRENT_LIST_DIR}/coordinate_type.h
     ${CMAKE_CURRENT_LIST_DIR}/euler_acceleration_axis_type.h
     ${CMAKE_CURRENT_LIST_DIR}/euler_rate_axis_type.h
     ${CMAKE_CURRENT_LIST_DIR}/euler_state_derivative_type.h
     ${CMAKE_CURRENT_LIST_DIR}/frame_state_type.h
     ${CMAKE_CURRENT_LIST_DIR}/frameState.cpp
     ${CMAKE_CURRENT_LIST_DIR}/frameState.h
     ${CMAKE_CURRENT_LIST_DIR}/interpolatedFrameState.cpp
     ${CMAKE_CURRENT_LIST_DIR}/interpolatedFrameState.h
     ${CMAKE_CURRENT_LIST_DIR}/interpolatedKinematicState.cpp
     ${CMAKE_CURRENT_LIST_DIR}/interpolatedKinematicState.h
     ${CMAKE_CURRENT_LIST_DIR}/kinematicState.cpp
     ${CMAKE_CURRENT_LIST_DIR}/kinematicState.h
     ${CMAKE_CURRENT_LIST_DIR}/motionState.cpp
     ${CMAKE_CURRENT_LIST_DIR}/motionState.h
     ${CMAKE_CURRENT_LIST_DIR}/motionStateContainer.cpp
     ${CMAKE_CURRENT_LIST_DIR}/motionStateContainer.h
     ${CMAKE_CURRENT_LIST_DIR}/motionStateMap.cpp
     ${CMAKE_CURRENT_LIST_DIR}/motionStateMap.h
     ${CMAKE_CURRENT_LIST_DIR}/position_axis_type.h
     ${CMAKE_CURRENT_LIST_DIR}/projectedFrameState.cpp
     ${CMAKE_CURRENT_LIST_DIR}/projectedFrameState.h
     ${CMAKE_CURRENT_LIST_DIR}/projectedKinematicState.cpp
     ${CMAKE_CURRENT_LIST_DIR}/projectedKinematicState.h
     ${CMAKE_CURRENT_LIST_DIR}/referenceFrame.cpp
     ${CMAKE_CURRENT_LIST_DIR}/referenceFrame.h
     ${CMAKE_CURRENT_LIST_DIR}/spherical_acceleration_axis_type.h
     ${CMAKE_CURRENT_LIST_DIR}/spherical_conversion_type.h
     ${CMAKE_CURRENT_LIST_DIR}/spherical_position_axis_type.h
     ${CMAKE_CURRENT_LIST_DIR}/spherical_velocity_axis_type.h
     ${CMAKE_CURRENT_LIST_DIR}/sphericalMotionState.cpp
     ${CMAKE_CURRENT_LIST_DIR}/sphericalMotionState.h
     ${CMAKE_CURRENT_LIST_DIR}/state_derivative_type.h
     ${CMAKE_CURRENT_LIST_DIR}/time_reference_type.h
     ${CMAKE_CURRENT_LIST_DIR}/velocity_axis_type.h
     PARENT_SCOPE)

