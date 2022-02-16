cmake_minimum_required (VERSION 3.7.0)

# add sources to the project
set (sources
     ${sources}
     ${CMAKE_CURRENT_LIST_DIR}/message_dispatcher.h
     ${CMAKE_CURRENT_LIST_DIR}/message_packet.h
     ${CMAKE_CURRENT_LIST_DIR}/message_recipient.h
     ${CMAKE_CURRENT_LIST_DIR}/messageConfiguration.cpp
     ${CMAKE_CURRENT_LIST_DIR}/messageConfiguration.h
     ${CMAKE_CURRENT_LIST_DIR}/messenger.h
     ${CMAKE_CURRENT_LIST_DIR}/priorityPublisher.cpp
     ${CMAKE_CURRENT_LIST_DIR}/priorityPublisher.h
     ${CMAKE_CURRENT_LIST_DIR}/priority_subscriber.h
     ${CMAKE_CURRENT_LIST_DIR}/publisher.cpp
     ${CMAKE_CURRENT_LIST_DIR}/publisher.h
     ${CMAKE_CURRENT_LIST_DIR}/signal_slot.h
     ${CMAKE_CURRENT_LIST_DIR}/static_message_dispatcher.h
     ${CMAKE_CURRENT_LIST_DIR}/subscriber.cpp
     ${CMAKE_CURRENT_LIST_DIR}/subscriber.h
     PARENT_SCOPE)

