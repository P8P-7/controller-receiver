#include "main.h"

Message joystickToMoveCommand(Control control) {
    Message message;
    auto *moveCommand(new MoveCommand);
    MotorCommand_gears gear;
    MotorCommand_motors motors[] {
        MotorCommand_motors_LEFT_FRONT,
        MotorCommand_motors_LEFT_BACK
    };


    if (control.value < 0) {
        gear = MotorCommand_gears_FORWARD;
    } else if (control.value > 0) {
        gear = MotorCommand_gears_BACKWARD;
    } else {
        gear = MotorCommand_gears_LOCK;
    }

    if (control.control == "JSLX" || control.control == "JSLY") {
        motors[0] = MotorCommand_motors_LEFT_FRONT;
        motors[1] = MotorCommand_motors_LEFT_BACK;
    } else {
        motors[0] = MotorCommand_motors_RIGHT_FRONT;
        motors[1] = MotorCommand_motors_RIGHT_BACK;
    }

    for (MotorCommand_motors motor : motors) {
        MotorCommand *motorCommand = moveCommand->add_commands();
        motorCommand->set_motor(motor);
        motorCommand->set_gear(gear);
        motorCommand->set_speed(control.value);
    }

    message.set_allocated_movecommand(moveCommand);

    return message;
}

Message buttonToMessage(Control control){
    //TODO button actions
}

Message convert(Control control) {
    switch (JOYSTICKMAP[control.control]) {
        case JSLX:
        case JSLY:
        case JSRX:
        case JSRY:
            return joystickToMoveCommand(control);
        case BTN1:
        case BTN2:
        case BTN3:
        case BTN4:
            break;
    }
}

int main() {
    const int BUFFER_SIZE = 1024;

    bool stop = false;

    const char *brokerAdress = "localhost";
    zmq::context_t context(1);
    io::zmq_publisher pub(context, brokerAdress, 5556);

    const char *device = "/dev/rfcomm1";
    btc::bluetooth_controller bt(device);
    boost::asio::serial_port port = bt.connect();

    while (!stop) {
        char c;
        auto *buffer = new char[BUFFER_SIZE];
        int i = 0;

        while (boost::asio::read(port, boost::asio::buffer(&c, 1)) == 1) {
            buffer[i++] = c;

            if (c == '}') {
                break; // End of command
            }
        }

        std::string command(buffer);
        std::size_t begin_pos = command.find('{');
        std::size_t end_pos = command.find('}');
        std::size_t separator_pos = command.find(':');

        Control control(
                command.substr(begin_pos + 1, separator_pos - begin_pos - 1),      // Substring between '{' and ':'
                command.substr(separator_pos + 1, end_pos - separator_pos - 1)); // Substring between ':' and '}'

        Message message = convert(control);

//        std::cout << std::endl << "control: " << control.control << std::endl << "state: " << control.value << std::endl << std::endl;

//         Send topic
         pub.publish(message);
    }
}
