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
        BT_LOG_ERROR,
        BT_LOG_FATAL
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

        /**
        * @brief Constructor
        */
        BluetoothController();

        /**
        * @fn bool connect()
        * @brief Connect to controller
        */
        bool connect();

        /**
        * @fn bool start()
        * @brief Start the controller
        */
        void start();

        /**
        * @overload bool start()
        * @brief Start the controller with new device
        * @param newDevicePath Path to bluetooth serial device
        * @param newDeviceAddress Bluetooth adress of controller
        */
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
        * @brief Send Data to controller
        * @param status Status to send
        * @param value value
        */
        void send(Status status, int value);

        /**
        * @overload void send(int severity, std::string message)
        * @brief Send log message to controller
        * @param severity log severity
        * @param message log message
        */
        void send(int severity, std::string message);

        /**
        * @fn void sendLast()
        * @brief Send last status message to controller
        */
        void sendLast();

        /**
        * @fn void clear()
        * @brief Clear bluetooth serial buffer
        */
        void clear();

        /**
        * @fn bool connected()
        * @brief Check if still connected to controller
        * @return bool true if connected to controller
        */
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
