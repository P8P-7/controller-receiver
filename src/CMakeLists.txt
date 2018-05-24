add_subdirectory(modules)

SET(SOURCES
        main.cpp
        map.h control.cpp control.h config.h convert.cpp convert.h config.cpp map.cpp)

find_package(Boost ${BOOST_MIN_VERSION}
        COMPONENTS
        log
        REQUIRED
        )
add_definitions(-DBOOST_LOG_DYN_LINK)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
        goliath-proto
        goliath-foundation-module
        goliath-zmq-messaging-module
        goliath-bluetooth_controller-module
        ${Boost_SYSTEM_LIBRARY}
        ${cppzmq_LIBRARY}
        ${Protobuf_LIBRARY})

configure_file(resources/converter-text.txt ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/converter-text.txt COPYONLY)