add_subdirectory(modules)

SET(SOURCES
        main.cpp
        )

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(controller_converter
        pthread
        goliath-proto
        goliath-io-module
        goliath-bluetooth_controller-module
        ${Boost_SYSTEM_LIBRARY}
        ${cppzmq_LIBRARY}
        ${Protobuf_LIBRARY})