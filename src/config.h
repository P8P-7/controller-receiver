#pragma once

#include <string>

/**
 * @file config.h
 * @author Group 7 - Informatica
 */

/**
 * @var enum CONFIG
 * @brief enum for all configurations
 */
enum CONFIG {
    MODE,
    SENSITIVITY,
};

/**
 * @fn CONFIG stringToConfig(std::string string)
 * @brief Converts config string from controller to CONFIG enum
 * @param string Button or axis
 */
CONFIG stringToConfig(std::string string);
