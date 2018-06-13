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
        BT_LOG_WARNING,
        BT_LOG_ERROR
    };

    /**
    * @brief enum for input errors
    */
    enum class InputError : std::uint8_t {
        IE_SUCCES,
        IE_EMPTY,
        IE_WRONG_FORMAT,
        IE_WRONG_VALUE,
        IE_READ_ERROR,
        IE_CONNECTION_LOST
    };

    struct Input {
    public:
        Input();

        Input(std::string type, std::string control, std::string value);

        Input(InputError error);

        std::string type;
        std::string control;
        std::string value;
        InputError error;
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
        BluetoothController(const std::string &devicePath, std::string &newDeviceAddress);

        BluetoothController();

        /**
        * @fn bool connect()
        * @brief Connect to controller
        */
        bool connect();

        void start();

        void start(const std::string &newDevicePath, std::string &newDeviceAddress);

        /**
        * @fn void reconnect()
        * @brief Reconnect to controller if connection was lost
        */
        void reconnect();

        /**
        * @fn std::tuple<std::string, std::string, std::string> receive()
        * @brief Wait for input to be received
        */
        Input receive();

        /**
        * @fn void send(Status status, short value)
        * @brief Reconnect to controller if connection was lost
         * @param status
        */
        void send(Status status, int value);

        void send(Status status, int severity, std::string message);

        void sendLast();

        void clear();

        bool connected();

    private:
        const char *devicePath;
        const int BUFFER_SIZE = 1024;
        std::string deviceAddress;
        boost::asio::io_service io;
        boost::asio::serial_port serialPort = boost::asio::serial_port(io);
        StatusMessage lastMessage = StatusMessage((Status) 0, 0);

        Input convertInput(char buffer[]);
    };
}
