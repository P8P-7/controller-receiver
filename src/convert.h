#pragma once

#include <iostream>
#include <MessageCarrier.pb.h>
#include <boost/log/trivial.hpp>
#include <deque>

#include "config.h"
#include "control.h"
#include "map.h"

using namespace goliath::proto;

/**
 * @var enum TYPE
 * @brief enum for all input types
 */
enum TYPE {
    CONTROL_TYPE = 0,
    CONFIG_TYPE = 1,
    LAST_STATUS_TYPE = 2
};

/**
 * @fn goliath::proto::MessageCarrier dualJoystickToMove(CONTROL control, int value)
 * @brief Converts joystick movement into Message with a MoveCommand.
 * @param control Button or axis
 * @param value Value of button or axis
 */
goliath::proto::MessageCarrier dualJoystickToMove(CONTROL control, int value);

/**
 * @fn goliath::proto::MessageCarrier buttonToFrontWing(CONTROL control, int value)
 * @brief Converts button presses to message with MoveWingCommand for front wings
 * @param control Button or axis
 * @param value Value of button or axis
 */
goliath::proto::MessageCarrier buttonToFrontWing(CONTROL control, int value);

/**
 * @fn goliath::proto::MessageCarrier buttonToBackWing(CONTROL control, int value)
 * @brief Converts button presses to message with MoveWingCommand for back wings
 * @param control Button or axis
 * @param value Value of button or axis
 */
goliath::proto::MessageCarrier buttonToBackWing(CONTROL control, int value);

/**
 * @fn goliath::proto::MessageCarrier buttonToAllWing(CONTROL control, int value)
 * @brief Converts button presses to message with MoveWingCommand for front/back wings
 * @param control Button or axis
 * @param value Value of button or axis
 */
goliath::proto::MessageCarrier buttonToAllWing(CONTROL control, int value);

/**
 * @fn goliath::proto::MessageCarrier convertControl(CONTROL control, int value, std::map<CONTROL, std::function<goliath::proto::MessageCarrier(CONTROL,int)>> function)
 * @brief Calls function mapped to the input
 * @param control Button or axis
 * @param value Value of button or axis
 */
goliath::proto::MessageCarrier convertControl(CONTROL control, int value, std::map<CONTROL, std::function<goliath::proto::MessageCarrier(CONTROL,int)>> function);

/**
 * @fn goliath::proto::MessageCarrier toMoveWingMessage(ServoCommand_Motor wing, ServoCommand_Direction direction, int speed)
 * @brief Converts input to Message with MoveWingCommand.
 * @param wing Wing
 * @param direction Direction to move the wing in
 * @param speed Speed to move the wing
 */
goliath::proto::MessageCarrier toMoveWingMessage(goliath::proto::commands::ServoCommand_Motor wing, goliath::proto::commands::ServoCommand_Direction direction, int speed);

/**
 * @fn goliath::proto::MessageCarrier toMoveWingMessage(std::deque<commands::ServoCommand_Motor> wings, commands::ServoCommand_Direction direction, int speed)
 * @brief Converts input to Message with MoveWingCommand.
 * @param wings Deque of wings
 * @param direction Direction to move the wing in
 * @param speed Speed to move the wing
 */
goliath::proto::MessageCarrier toMoveWingMessage(std::vector<commands::ServoCommand_Motor> wings, commands::ServoCommand_Direction direction, int speed);

/**
 * @fn TYPE stringToType (std::string string)
 * @brief Converts type string from controller to TYPE enum
 * @param string Type of input
 */
TYPE stringToType (std::string string);