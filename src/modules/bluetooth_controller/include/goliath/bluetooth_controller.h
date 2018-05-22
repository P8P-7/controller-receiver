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

    enum STATUS {
        BT_CONNECTED,
        BT_INVALID_INPUT,
        BT_BATTERY,
    };

    struct statusMessage {
        statusMessage(STATUS status, int value);

        STATUS status;
        int value;
    };

    /**
    * @class goliath::btc::BluetoothController
    */
    class BluetoothController {
    public:
        /**
        * @fn goliath::btc::BluetoothController(const char *dev)
        * @brief Constructor
         * @param dev Path to bluetooth serial device
        */
        BluetoothController(const char *dev);

        /**
        * @fn tuple<std::string, std::string, std::string> goliath::btc::std::receive()
        * @brief Wait for input to be received
        */
        std::tuple<std::string, std::string, std::string> receive();

        void send(STATUS status, int value);

    private:
        std::tuple<std::string, std::string, std::string> convertInput(char buffer[]);

        boost::asio::io_service io;
        boost::asio::serial_port serialPort = boost::asio::serial_port(io);
        const int BUFFER_SIZE = 1024;
    };
}
