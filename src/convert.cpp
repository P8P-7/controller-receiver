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

    BOOST_LOG_TRIVIAL(debug) << "Axis \"" << control << "\" moved to \"" << value << "\".";

    // Apply sensitivity
    value = value * CONFIGURATION[SENSITIVITY] / 255;

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
    MessageCarrier message;
    auto *commandMessage(new CommandMessage);
    auto *moveCommand(new MoveWingCommand);

    ServoCommand_Motor wing;
    ServoCommand_Direction direction;
    int speed = 0;
    if(value == 1){
        speed = 512;
    }

    switch (control) {
        case BTN1:
            wing = ServoCommand_Motor_LEFT_FRONT;
            direction = ServoCommand_Direction_UP;
            break;
        case BTN2:
            wing = ServoCommand_Motor_LEFT_FRONT;
            direction = ServoCommand_Direction_DOWN;
            break;
        case BTN3:
            wing = ServoCommand_Motor_RIGHT_FRONT;
            direction = ServoCommand_Direction_UP;
            break;
        case BTN4:
            wing = ServoCommand_Motor_RIGHT_FRONT;
            direction = ServoCommand_Direction_DOWN;
            break;
    }

    ServoCommand *wingCommand = moveCommand->add_commands();
    wingCommand->set_motor(wing);
    wingCommand->set_speed(speed);
    wingCommand->set_direction(direction);

    commandMessage->set_allocated_movewingcommand(moveCommand);
    message.set_allocated_commandmessage(commandMessage);

    if(value == 0){
        BOOST_LOG_TRIVIAL(debug) << "Button \"" << control - 4 << "\" released.";
    } else{
        BOOST_LOG_TRIVIAL(debug) << "Button \"" << control - 4 << "\" pressed.";
    }
    return message;
}

MessageCarrier buttonToBackWing(CONTROL control, int value) {
    MessageCarrier message;
    auto *commandMessage(new CommandMessage);
    auto *moveCommand(new MoveWingCommand);

    ServoCommand_Motor wing;
    ServoCommand_Direction direction;
    int speed = 0;
    if(value == 1){
        speed = 512;
    }

    switch (control) {
        case BTN1:
            wing = ServoCommand_Motor_LEFT_BACK;
            direction = ServoCommand_Direction_UP;
            break;
        case BTN2:
            wing = ServoCommand_Motor_LEFT_BACK;
            direction = ServoCommand_Direction_DOWN;
            break;
        case BTN3:
            wing = ServoCommand_Motor_RIGHT_BACK;
            direction = ServoCommand_Direction_UP;
            break;
        case BTN4:
            wing = ServoCommand_Motor_RIGHT_BACK;
            direction = ServoCommand_Direction_DOWN;
            break;
    }

    ServoCommand *wingCommand = moveCommand->add_commands();
    wingCommand->set_motor(wing);
    wingCommand->set_speed(speed);
    wingCommand->set_direction(direction);

    commandMessage->set_allocated_movewingcommand(moveCommand);
    message.set_allocated_commandmessage(commandMessage);

    if(value == 0){
        BOOST_LOG_TRIVIAL(debug) << "Button \"" << control - 4 << "\" released.";
    } else{
        BOOST_LOG_TRIVIAL(debug) << "Button \"" << control - 4 << "\" pressed.";
    }
    return message;
}

MessageCarrier convertControl(CONTROL control, int value, std::map<CONTROL, std::function<MessageCarrier(CONTROL,int)>> functionMap) {
    return functionMap[control](control,value);
}

TYPE stringToType(std::string string) {
    return static_cast<TYPE>(atoi(string.c_str()));
}
