#pragma once

#include <string>
#include <MessageCarrier.pb.h>
#include <functional>
#include "config.h"
#include "control.h"

/**
 * @file map.h
 * @author Group 7 - Informatica
 */

/**
 * @fn void initConfig()
 * @brief Initializes controller configuration.
 */
void initConfig();

/**
 * @fn int getConfig(CONFIG config)
 * @brief Get config value.
 * @param config CONFIG where you want to get the value for.
 */
int getConfig(CONFIG config);

/**
 * @fn void setConfig(CONFIG config, int value)
 * @brief Get config value.
 * @param config config you want to change the set of.
 * @param value value to set for config.
 */
void setConfig(CONFIG config, int value);

/**
 * @var std::map<CONTROL, std::function<Message(CONTROL,int)>> FUNCTION_MAP
 * @brief function to input map
 */
static std::map<CONTROL, std::function<MessageCarrier(CONTROL,int)>> FUNCTION_MAP;

/**
 * @var std::map<CONFIG, int> CONFIGURATION
 * @brief configuration to value map
 */
static std::map<CONFIG, int> CONFIGURATION;
