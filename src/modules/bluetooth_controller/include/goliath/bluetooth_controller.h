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
   * @brief enum for all status updates to controller
   */
    enum class Status : std::uint8_t {
        BT_CONNECTED,
        BT_INVALID_INPUT,
        BT_BATTERY,
    };

    struct StatusMessage {
        StatusMessage();
        StatusMessage(Status status, short value);
        void set(Status status, short value);

        Status status;
        short value;
        bool isSet = false;
    };

    /**
    * @class goliath::btc::BluetoothController
    */
    class BluetoothController {
    public:
        /**
        * @brief Constructor
        * @param dev Path to bluetooth serial device
        */
        BluetoothController(const std::string &devicePath);

        /**
        * @brief Wait for input to be received
        */
        std::tuple<std::string, std::string, std::string> receive();

        void send(Status status, short value);

        void sendLast();

        void clear();

    private:
        std::tuple<std::string, std::string, std::string> convertInput(char buffer[]);

        boost::asio::io_service io;
        boost::asio::serial_port serialPort = boost::asio::serial_port(io);
        const int BUFFER_SIZE = 1024;
        StatusMessage lastMessage = StatusMessage((Status)0,0);
    };
}
