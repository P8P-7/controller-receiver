//
// Created by hidde on 9-5-18.
//

#ifndef CONTROLLER_CONVERTER_CONVERTER_H
#define CONTROLLER_CONVERTER_CONVERTER_H

#include <string>
#include <iostream>
#include <fstream>

#include <zmq.hpp>

#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include "modules/bluetooth_controller/include/goliath/bluetooth_controller.h"
#include "modules/io/include/goliath/zmq_io.h"
#include "modules/io/include/goliath/zmq_publisher.h"

using namespace goliath;

struct Control {
    std::string control;
    int value;
public:
    Control(std::string con, int val){
        control = con;
        value = val;
    }
    Control(std::string con, std::string val){
        control = con;
        value = atoi(val.c_str());
    }
};

int main();

#endif //CONTROLLER_CONVERTER_CONVERTER_H
