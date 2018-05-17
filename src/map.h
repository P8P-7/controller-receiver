#pragma once

#include <string>
#include <Message.pb.h>
#include <functional>
#include "config.h"
#include "control.h"

/**
 * @file map.h
 * @author Group 7 - Informatica
 */

/**
 * @var std::map<CONTROL, std::function<Message(CONTROL,int)>> FUNCTION_MAP
 * @brief function to input map
 */
static std::map<CONTROL, std::function<Message(CONTROL,int)>> FUNCTION_MAP;

/**
 * @var std::map<CONFIG, int> CONFIGURATION
 * @brief configuration to value map
 */
static std::map<CONFIG, int> CONFIGURATION;
