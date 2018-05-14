//
// Created by hidde on 12-5-18.
//

#include "include/goliath/bluetooth_controller.h"

using namespace goliath::btc;

bluetooth_controller::bluetooth_controller(std::string dev){
    device = dev;
}

boost::asio::serial_port bluetooth_controller::connect() {
    boost::asio::serial_port port(io,device);
    port.set_option(boost::asio::serial_port_base::baud_rate(9600)); // Default for bluetooth
    port.set_option(boost::asio::serial_port_base::character_size(8));
    return port;
}

