#pragma once

#include <iostream>

#include "config.h"
#include "control.h"
#include "map.h"
#include "Message.pb.h"

Message joystickToMoveCommand(Control control);

Message buttonToMessage(Control control);

//Message convertControl(Control control);
Message convertControl(Control control, std::map<CONTROL, std::function<Message(Control)>> function);