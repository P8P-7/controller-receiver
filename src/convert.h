#pragma once

#include <iostream>
#include <MessageCarrier.pb.h>
#include <boost/log/trivial.hpp>

#include "config.h"
#include "control.h"
#include "map.h"

enum TYPE {
    CONTROL_TYPE = 0,
    CONFIG_TYPE = 1,
};

MessageCarrier dualJoystickToMove(CONTROL control, int value);

MessageCarrier buttonToFrontArm(CONTROL control, int value);

MessageCarrier convertControl(CONTROL control, int value, std::map<CONTROL, std::function<MessageCarrier(CONTROL,int)>> function);

TYPE stringToType (std::string string);