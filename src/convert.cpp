#include "convert.h"

Message joystickToMoveCommand(Control control) {
    Message message;
    auto *moveCommand(new MoveCommand);
    MotorCommand_gears gear;
    MotorCommand_motors motors[] {
            MotorCommand_motors_LEFT_FRONT,
            MotorCommand_motors_LEFT_BACK
    };

    // Apply sensitivity
    control.value = static_cast<int>((float)control.value * ((float)CONFIGURATION[SENSITIVITY] / 255.0));

    // Select gear
    if (control.value < 0) {
        gear = MotorCommand_gears_FORWARD;
    } else if (control.value > 0) {
        gear = MotorCommand_gears_BACKWARD;
    } else {
        gear = MotorCommand_gears_LOCK;
    }

    // Select motors
    if (control.control == JSLX || control.control == JSLY) {
        motors[0] = MotorCommand_motors_LEFT_FRONT;
        motors[1] = MotorCommand_motors_LEFT_BACK;
    } else if (control.control == JSRX || control.control == JSRY){
        motors[0] = MotorCommand_motors_RIGHT_FRONT;
        motors[1] = MotorCommand_motors_RIGHT_BACK;
    }

    // Add motor commands to single move command
    for (MotorCommand_motors motor : motors) {
        MotorCommand *motorCommand = moveCommand->add_commands();
        motorCommand->set_motor(motor);
        motorCommand->set_gear(gear);
        motorCommand->set_speed(control.value);
    }

    // Put move command in a message
    message.set_allocated_movecommand(moveCommand);

    return message;
}

Message buttonToMessage(Control control) {
    //TODO button actions
}

Message convertControl(Control control, std::map<CONTROL, std::function<Message(Control)>> functionMap) {;
    Message message = functionMap[control.control](control);
    return message;
}