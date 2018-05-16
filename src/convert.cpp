#include "convert.h"

Message dualJoystickToMove(CONTROL control, int value) {
    Message message;
    auto *moveCommand(new MoveCommand);
    MotorCommand_gears gear;
    MotorCommand_motors motors[] {
            MotorCommand_motors_LEFT_FRONT,
            MotorCommand_motors_LEFT_BACK
    };

    // Apply sensitivity
    value = static_cast<int>((float)value * ((float)CONFIGURATION[SENSITIVITY] / 255.0));

    // Select gear
    if (value < 0) {
        gear = MotorCommand_gears_FORWARD;
    } else if (value > 0) {
        gear = MotorCommand_gears_BACKWARD;
    } else {
        gear = MotorCommand_gears_LOCK;
    }

    // Select motors
    if (control == JSLX || control == JSLY) {
        motors[0] = MotorCommand_motors_LEFT_FRONT;
        motors[1] = MotorCommand_motors_LEFT_BACK;
    } else if (control == JSRX || control == JSRY){
        motors[0] = MotorCommand_motors_RIGHT_FRONT;
        motors[1] = MotorCommand_motors_RIGHT_BACK;
    }

    // Add motor commands to single move command
    for (MotorCommand_motors motor : motors) {
        MotorCommand *motorCommand = moveCommand->add_commands();
        motorCommand->set_motor(motor);
        motorCommand->set_gear(gear);
        motorCommand->set_speed(value);
    }

    // Put move command in a message
    message.set_allocated_movecommand(moveCommand);

    return message;
}

Message buttonToMessage(CONTROL control, int value) {
    //TODO button actions
}

Message convertControl(CONTROL control, int value, std::map<CONTROL, std::function<Message(CONTROL,int)>> functionMap) {;
    return functionMap[control](control,value);
}

TYPE stringToType(std::string string) {
    return static_cast<TYPE>(atoi(string.c_str()));
}
