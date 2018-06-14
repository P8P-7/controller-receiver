#pragma once

#include <string>

/**
 * @file control.h
 * @author Group 7 - Informatica
 */

/**
 * @var enum CONTROL
 * @brief enum for all axes and buttons
 */
enum CONTROL {
    JSLX, JSLY, JSRX, JSRY, BTN1, BTN2, BTN3, BTN4, CONTROL_NR_ITEMS
};

/**
 * @fn CONTROL stringToControl(std::string string)
 * @brief Converts control string from controller to CONTROL enum
 * @param string Button or axis
 */
CONTROL stringToControl(std::string string);

/**
 * @fn int stringToValue(std::string string);
 * @brief Converts value of button, axis or config from string to int
 * @param string value of button, axis or config
 */
int stringToValue(std::string string);
