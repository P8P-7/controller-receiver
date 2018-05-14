project(goliath-bluetooth_controller-module LANGUAGES CXX)

set(SOURCES
        include/goliath/bluetooth_controller.h
        bluetooth_controller.cpp
        )

add_library(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        )

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        goliath-proto
        PRIVATE
        ${Protobuf_LIBRARY}
        )