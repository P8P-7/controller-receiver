#pragma once

#include <string>
#include <Message.pb.h>
#include <functional>
#include "config.h"
#include "control.h"

static std::map<CONTROL, std::function<Message(Control)>> FUNCTION_MAP;

static std::map<CONFIG, int> CONFIGURATION;
