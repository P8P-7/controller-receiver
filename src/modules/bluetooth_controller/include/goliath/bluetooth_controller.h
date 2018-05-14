//
// Created by hidde on 12-5-18.
//

#ifndef CONTROLLER_CONVERTER_BLUETOOTH_CONTROLLER_H
#define CONTROLLER_CONVERTER_BLUETOOTH_CONTROLLER_H

#include <string>

#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>

namespace goliath::btc {
    class bluetooth_controller {
    public:
        bluetooth_controller(std::string);
        boost::asio::serial_port connect();

    private:
        std::string device;
        boost::asio::io_service io;
    };
}


#endif //CONTROLLER_CONVERTER_BLUETOOTH_CONTROLLER_H
