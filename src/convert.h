#pragma once

#include <iostream>

#include "config.h"
#include "control.h"
#include "map.h"
#include "Message.pb.h"

enum TYPE {
    CONTROL_TYPE = 0,
    CONFIG_TYPE = 1,
};

Message dualJoystickToMove(CONTROL control, int value);

Message buttonToMessage(CONTROL control, int value);

Message convertControl(CONTROL control, int value, std::map<CONTROL, std::function<Message(CONTROL,int)>> function);

TYPE stringToType (std::string string);