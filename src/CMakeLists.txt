add_subdirectory(modules)

SET(SOURCES
        main.cpp
        map.h control.cpp control.h config.h convert.cpp convert.h config.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(controller_converter
        goliath-proto
        goliath-io-module
        goliath-bluetooth_controller-module
        ${Boost_SYSTEM_LIBRARY}
        ${cppzmq_LIBRARY}
        ${Protobuf_LIBRARY})