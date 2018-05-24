#pragma once

#include <string>

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
