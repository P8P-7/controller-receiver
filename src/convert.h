#pragma once

#include <iostream>
#include <MessageCarrier.pb.h>
#include <boost/log/trivial.hpp>

#include "config.h"
#include "control.h"
#include "map.h"

/**
 * @var enum TYPE
 * @brief enum for all input types
 */
enum TYPE {
    IGNORE_TYPE = -3,
    CONNECTIONLOST_TYPE = -2,
    CONTROL_TYPE = 0,
    CONFIG_TYPE = 1,
    LASTSTATUS_TYPE = 2
};

MessageCarrier ignoreInput(CONTROL control, int value);

/**
 * @fn MessageCarrier dualJoystickToMove(CONTROL control, int value)
 * @brief Converts joystick movement into Message with a MoveCommand.
 * @param control Button or axis
 * @param value Value of button or axis
 */
MessageCarrier dualJoystickToMove(CONTROL control, int value);

/**
 * @fn MessageCarrier buttonToFrontWing(CONTROL control, int value)
 * @brief Converts button presses to message with MoveWingCommand for front wings
 * @param control Button or axis
 * @param value Value of button or axis
 */
MessageCarrier buttonToFrontWing(CONTROL control, int value);

/**
 * @fn MessageCarrier buttonToBackWing(CONTROL control, int value)
 * @brief Converts button presses to message with MoveWingCommand for back wings
 * @param control Button or axis
 * @param value Value of button or axis
 */
MessageCarrier buttonToBackWing(CONTROL control, int value);

/**
 * @fn MessageCarrier convertControl(CONTROL control, int value, std::map<CONTROL, std::function<MessageCarrier(CONTROL,int)>> function)
 * @brief Calls function mapped to the input
 * @param control Button or axis
 * @param value Value of button or axis
 */
MessageCarrier convertControl(CONTROL control, int value, std::map<CONTROL, std::function<MessageCarrier(CONTROL,int)>> function);

/**
 * @fn MessageCarrier toMoveWingMessage(ServoCommand_Motor wing, ServoCommand_Direction direction, int speed)
 * @brief Converts input to Message with MoveWingCommand.
 * @param wing Wing
 * @param direction Direction to move the wing in
 * @param speed Speed to move the wing
 */
MessageCarrier toMoveWingMessage(ServoCommand_Motor wing, ServoCommand_Direction direction, int speed);

/**
 * @fn TYPE stringToType (std::string string)
 * @brief Converts type string from controller to TYPE enum
 * @param string Type of input
 */
TYPE stringToType (std::string string);