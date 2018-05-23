#include "convert.h"

MessageCarrier dualJoystickToMove(CONTROL control, int value) {
    MessageCarrier message;
    auto *commandMessage(new CommandMessage);
    auto *moveCommand(new MoveCommand);
    MotorCommand_Gear gear;
    MotorCommand_Motor motors[] {
            MotorCommand_Motor_LEFT_FRONT,
            MotorCommand_Motor_LEFT_BACK
    };

//    BOOST_LOG_TRIVIAL(debug) << "Axis \"" << control << "\" moved to \"" << value << "\".";

    // Apply sensitivity
    value = value * getConfig(SENSITIVITY) / 255;

    // Select gear
    if (value < 0) {
        gear = MotorCommand_Gear_FORWARD;
    } else if (value > 0) {
        gear = MotorCommand_Gear_BACKWARD;
    } else {
        gear = MotorCommand_Gear_LOCK;
    }

    // Select motors
    if (control == JSLX || control == JSLY) {
        motors[0] = MotorCommand_Motor_LEFT_FRONT;
        motors[1] = MotorCommand_Motor_LEFT_BACK;
    } else if (control == JSRX || control == JSRY){
        motors[0] = MotorCommand_Motor_RIGHT_FRONT;
        motors[1] = MotorCommand_Motor_RIGHT_BACK;
    }

    // Add motor commands to single move command
    for (MotorCommand_Motor motor : motors) {
        MotorCommand *motorCommand = moveCommand->add_commands();
        motorCommand->set_motor(motor);
        motorCommand->set_gear(gear);
        motorCommand->set_speed(value);
    }

    // Put move command in a message
    commandMessage->set_allocated_movecommand(moveCommand);
    message.set_allocated_commandmessage(commandMessage);

    BOOST_LOG_TRIVIAL(debug) << "Axis \"" << control << "\" moved to \"" << value << "\".";

    return message;
}

MessageCarrier buttonToFrontWing(CONTROL control, int value) {
    ServoCommand_Motor wing;
    ServoCommand_Direction direction;
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
            return toMoveWingMessage(ServoCommand_Motor_LEFT_FRONT, ServoCommand_Direction_UP, speed);
        case BTN2:
            return toMoveWingMessage(ServoCommand_Motor_LEFT_FRONT, ServoCommand_Direction_DOWN, speed);
        case BTN3:
            return toMoveWingMessage(ServoCommand_Motor_RIGHT_FRONT, ServoCommand_Direction_UP, speed);
        case BTN4:
            return toMoveWingMessage(ServoCommand_Motor_RIGHT_FRONT, ServoCommand_Direction_DOWN, speed);
        default:
            break;
    }

    return MessageCarrier();
}

MessageCarrier buttonToBackWing(CONTROL control, int value) {

    ServoCommand_Motor wing;
    ServoCommand_Direction direction;
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
            return toMoveWingMessage(ServoCommand_Motor_LEFT_BACK, ServoCommand_Direction_UP, speed);
        case BTN2:
            return toMoveWingMessage(ServoCommand_Motor_LEFT_BACK, ServoCommand_Direction_DOWN, speed);
        case BTN3:
            return toMoveWingMessage(ServoCommand_Motor_RIGHT_BACK, ServoCommand_Direction_UP, speed);
        case BTN4:
            return toMoveWingMessage(ServoCommand_Motor_RIGHT_BACK, ServoCommand_Direction_DOWN, speed);
        default:
            break;
    }

    return MessageCarrier();
}

MessageCarrier convertControl(CONTROL control, int value, std::map<CONTROL, std::function<MessageCarrier(CONTROL,int)>> functionMap) {
    return functionMap[control](control,value);
}

MessageCarrier toMoveWingMessage(ServoCommand_Motor wing, ServoCommand_Direction direction, int speed){
    MessageCarrier message;
    auto *commandMessage(new CommandMessage);
    auto *moveCommand(new MoveWingCommand);

    ServoCommand *wingCommand = moveCommand->add_commands();
    wingCommand->set_motor(wing);
    wingCommand->set_speed(speed);
    wingCommand->set_direction(direction);

    commandMessage->set_allocated_movewingcommand(moveCommand);
    message.set_allocated_commandmessage(commandMessage);

    return message;
}

TYPE stringToType(std::string string) {
    return static_cast<TYPE>(atoi(string.c_str()));
}
