#include <deque>
#include "convert.h"

using namespace goliath::proto;

MessageCarrier ignoreInput(CONTROL control, int value) {
    return MessageCarrier();
}

MessageCarrier dualJoystickToMove(CONTROL control, int value) {
    MessageCarrier message;
    CommandMessage *commandMessage(new CommandMessage);
    commands::MoveCommand *moveCommand(new commands::MoveCommand);
    commands::MotorCommand_Gear gear;
    std::deque<commands::MotorCommand_Motor> motors;

    // Apply sensitivity
    value = value * getConfig(SENSITIVITY) / 255 / 2;

    // Select gear
    if (value < 0) {
        gear = commands::MotorCommand_Gear_FORWARD;
    } else if (value > 0) {
        gear = commands::MotorCommand_Gear_BACKWARD;
    } else {
        gear = commands::MotorCommand_Gear_LOCK;
    }

    // Select motors
    if (control == JSLY) {
        motors.emplace_back(commands::MotorCommand_Motor_LEFT_FRONT);
        motors.emplace_back(commands::MotorCommand_Motor_LEFT_BACK);
    } else if (control == JSRY){
        motors.emplace_back(commands::MotorCommand_Motor_RIGHT_FRONT);
        motors.emplace_back(commands::MotorCommand_Motor_RIGHT_BACK);
    }

    // Add motor commands to single move command
    for (commands::MotorCommand_Motor motor : motors) {
        commands::MotorCommand *motorCommand = moveCommand->add_commands();
        motorCommand->set_motor(motor);
        motorCommand->set_gear(gear);
        motorCommand->set_speed(abs(value));
    }

    // Put move command in a message
    commandMessage->set_allocated_movecommand(moveCommand);
    message.set_allocated_commandmessage(commandMessage);

    BOOST_LOG_TRIVIAL(debug) << "Axis \"" << control << "\" moved to \"" << value << "\".";

    return message;
}

MessageCarrier buttonToFrontWing(CONTROL control, int value) {
    commands::ServoCommand_Motor wing;
    commands::ServoCommand_Direction direction;
    int speed = 0;
    if(value == 1){
        speed = 512;
    }

    if(value == 0){
        BOOST_LOG_TRIVIAL(debug) << "Button \"" << control - 4 << "\" released.";
    } else{
        BOOST_LOG_TRIVIAL(debug) << "Button \"" << control - 4 << "\" pressed.";
    }

    switch (control) {
        case BTN1:
            return toMoveWingMessage(commands::ServoCommand_Motor_LEFT_FRONT, commands::ServoCommand_Direction_UP, speed);
        case BTN2:
            return toMoveWingMessage(commands::ServoCommand_Motor_LEFT_FRONT, commands::ServoCommand_Direction_DOWN, speed);
        case BTN3:
            return toMoveWingMessage(commands::ServoCommand_Motor_RIGHT_FRONT, commands::ServoCommand_Direction_UP, speed);
        case BTN4:
            return toMoveWingMessage(commands::ServoCommand_Motor_RIGHT_FRONT, commands::ServoCommand_Direction_DOWN, speed);
        default:
            break;
    }

    return MessageCarrier();
}

MessageCarrier buttonToBackWing(CONTROL control, int value) {

    commands::ServoCommand_Motor wing;
    commands::ServoCommand_Direction direction;
    int speed = 0;
    if(value == 1){
        speed = 512;
    }

    if(value == 0){
        BOOST_LOG_TRIVIAL(debug) << "Button \"" << control - 4 << "\" released.";
    } else{
        BOOST_LOG_TRIVIAL(debug) << "Button \"" << control - 4 << "\" pressed.";
    }

    switch (control) {
        case BTN1:
            return toMoveWingMessage(commands::ServoCommand_Motor_LEFT_BACK, commands::ServoCommand_Direction_UP, speed);
        case BTN2:
            return toMoveWingMessage(commands::ServoCommand_Motor_LEFT_BACK, commands::ServoCommand_Direction_DOWN, speed);
        case BTN3:
            return toMoveWingMessage(commands::ServoCommand_Motor_RIGHT_BACK, commands::ServoCommand_Direction_UP, speed);
        case BTN4:
            return toMoveWingMessage(commands::ServoCommand_Motor_RIGHT_BACK, commands::ServoCommand_Direction_DOWN, speed);
        default:
            break;
    }

    return MessageCarrier();
}

MessageCarrier convertControl(CONTROL control, int value, std::map<CONTROL, std::function<MessageCarrier(CONTROL,int)>> functionMap) {
    if(functionMap[control] != NULL) {
        return functionMap[control](control, value);
    }
    return MessageCarrier();
}

MessageCarrier toMoveWingMessage(commands::ServoCommand_Motor wing, commands::ServoCommand_Direction direction, int speed){
    MessageCarrier message;
    auto *commandMessage(new CommandMessage);
    auto *moveCommand(new commands::MoveWingCommand);

    commands::ServoCommand *wingCommand = moveCommand->add_commands();
    wingCommand->set_motor(wing);
    wingCommand->set_speed(speed);
    wingCommand->set_direction(direction);

    commandMessage->set_allocated_movewingcommand(moveCommand);
    message.set_allocated_commandmessage(commandMessage);

    return message;
}

TYPE stringToType(std::string string) {
    return static_cast<TYPE>(std::stoi(string.c_str()));
}
