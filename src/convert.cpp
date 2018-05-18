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

    // Apply sensitivity
    value = static_cast<int>((float)value * ((float)CONFIGURATION[SENSITIVITY] / 255.0));

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

    return message;
}

MessageCarrier buttonToMessage(CONTROL control, int value) {
    //TODO button actions
}

MessageCarrier convertControl(CONTROL control, int value, std::map<CONTROL, std::function<MessageCarrier(CONTROL,int)>> functionMap) {
    return functionMap[control](control,value);
}

TYPE stringToType(std::string string) {
    return static_cast<TYPE>(atoi(string.c_str()));
}
