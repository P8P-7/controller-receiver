project(goliath-bluetooth_controller-module LANGUAGES CXX)

set(SOURCES
        include/goliath/bluetooth_controller.h
        bluetooth_controller.cpp
        )

add_library(${PROJECT_NAME} ${SOURCES})

add_definitions(-DBOOST_LOG_DYN_LINK)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        )

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        goliath-proto
        PRIVATE
        ${Boost_LIBRARIES}
        ${Protobuf_LIBRARY}
        )