#include "main.h"

void initControls() {
    FUNCTIONMAP.insert(std::make_pair(JSLX,joystickToMoveCommand));
    FUNCTIONMAP.insert(std::make_pair(JSLY,joystickToMoveCommand));
    FUNCTIONMAP.insert(std::make_pair(JSRX,joystickToMoveCommand));
    FUNCTIONMAP.insert(std::make_pair(JSRY,joystickToMoveCommand));
}

Message joystickToMoveCommand(Control control) {
    std::cout << control.control << ':' << control.value << std::endl;
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
    return FUNCTIONMAP[CONTROLMAP[control.control]](control);
}

int main(int argc, char **argv) {
    const int BUFFER_SIZE = 1024;
    const char *brokerAdress = "localhost";
    const char *device = "/dev/rfcomm1";
    bool stop = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "-a") || (arg == "--adress")) {
            if (i + 1 < argc) {
                brokerAdress = argv[i++];
            } else {
                std::cerr << "--adress option requires one argument." << std::endl;
                return 1;
            }
        } else if ((arg == "-d") || (arg == "--device")) {
            if (i + 1 < argc) {
                device = argv[i++];
            } else {
                std::cerr << "--device option requires one argument." << std::endl;
                return 1;
            }
        } else {
        }
    }

    initControls();

    zmq::context_t context(1);
    io::zmq_publisher pub(context, brokerAdress, 5556);

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

//      Send topic
        pub.publish(message);
    }
}
