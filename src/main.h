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
//#include "modules/io/include/goliath/zmq_io.h"
#include "modules/io/include/goliath/zmq_publisher.h"
#include "Message.pb.h"

using namespace goliath;

enum CONTROL{
    JSLX = 0,
    JSLY = 1,
    JSRX = 2,
    JSRY = 3,
    BTN1 = 4,
    BTN2 = 5,
    BTN3 = 6,
    BTN4 = 7,
};

struct Control {
    CONTROL control;
    int value;
public:
    Control(CONTROL con, int val){
        control = con;
        value = val;
    }
    Control(CONTROL con, std::string val){
        control = con;
        value = atoi(val.c_str());
    }
};

static std::map<std::string, CONTROL> CONTROL_MAP;

static std::map<CONTROL , std::function<Message(Control)>> FUNCTION_MAP;

// Functions:

void initControls();

Message joystickToMoveCommand(Control control);

Message buttonToMessage(Control control);

Message convert(Control control);

int main(int argc, char *argv[]);

#endif //CONTROLLER_CONVERTER_CONVERTER_H
