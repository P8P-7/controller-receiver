#pragma once

#include <string>
#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/log/trivial.hpp>

/**
 * @file bluetooth_controller.h
 * @author Group 7 - Informatica
 */

/**
 * @namespace goliath::btc
 * @brief Module for bluetooth communication with controller
 */

namespace goliath::btc {
    /**
    * @class goliath::btc::BluetoothController
    */
    class BluetoothController {
    public:
        /**
        * @fn goliath::btc
        * @brief Module for bluetooth communication with controller
        */
        BluetoothController(const char *);

        /**
        * @fn goliath::btc
        * @brief Module for bluetooth communication with controller
        */
        std::tuple<std::string, std::string, std::string> receive();
        void send(std::tuple<std::string, std::string, std::string>);

    private:
        std::tuple<std::string, std::string, std::string> convertInput(char buffer[]);
        boost::asio::io_service io;
        boost::asio::serial_port serialPort = boost::asio::serial_port(io);
        const int BUFFER_SIZE = 1024;
    };
}
