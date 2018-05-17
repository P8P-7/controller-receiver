#pragma once

#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>


namespace goliath::btc {
    class BluetoothController {
    public:
        BluetoothController(const char*);
        std::tuple<std::string,std::string,std::string> receive(bool debug);

    private:
        boost::asio::io_service io;
        boost::asio::serial_port serialPort = boost::asio::serial_port(io);
        const int BUFFER_SIZE = 1024;
    };
}
