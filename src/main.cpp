#include "main.h"

void initControls() {
    // Map function to controller input
    FUNCTION_MAP.insert(std::make_pair(JSLX,joystickToMoveCommand));
    FUNCTION_MAP.insert(std::make_pair(JSLY,joystickToMoveCommand));
    FUNCTION_MAP.insert(std::make_pair(JSRX,joystickToMoveCommand));
    FUNCTION_MAP.insert(std::make_pair(JSRY,joystickToMoveCommand));
    FUNCTION_MAP.insert(std::make_pair(BTN1,buttonToMessage));
    FUNCTION_MAP.insert(std::make_pair(BTN2,buttonToMessage));
    FUNCTION_MAP.insert(std::make_pair(BTN3,buttonToMessage));
    FUNCTION_MAP.insert(std::make_pair(BTN4,buttonToMessage));
}

Message joystickToMoveCommand(Control control) {
    Message message;
    auto *moveCommand(new MoveCommand);
    MotorCommand_gears gear;
    MotorCommand_motors motors[] {
            MotorCommand_motors_LEFT_FRONT,
            MotorCommand_motors_LEFT_BACK
    };

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

Message convert(Control control) {
    return FUNCTION_MAP[control.control](control);
}

static void show_usage(std::string name)
{
    std::cerr << "Usage: " << name << " <option(s)> ARGUMENT\n"
              << "Options:\n"
              << "\t-h,--help\t\tShow this help message\n"
              << "\t-a,--address IP\t\tSpecify the broker ip-address\n"
              << "\t-d,--device DEVICE\tSpecify the device path"
              << std::endl;
}

int main(int argc, char **argv) {
    const int BUFFER_SIZE = 1024;
    const char *brokerAdress = "localhost";
    const char *device = "/dev/rfcomm1";
    bool stop = false;
    bool debug = false;

    // Command line options
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "-h") || (arg == "--help")) {
            show_usage(argv[0]);
            return 0;
        }
        else if ((arg == "-a") || (arg == "--address")) {
            if (i + 1 < argc) {
                i++;
                brokerAdress = argv[i];
            } else {
                std::cerr << "--address option requires one argument." << std::endl;
                return 1;
            }
        } else if ((arg == "-d") || (arg == "--device")) {
            if (i + 1 < argc) {
                i++;
                device = argv[i];
            } else {
                std::cerr << "--device option requires one argument." << std::endl;
                return 1;
            }
        } else if (arg == "--debug") {
            debug = true;
        }
    }

    // Set controls
    initControls();


    // Setup ZMQ publisher
    zmq::context_t context(1);
    io::zmq_publisher pub(context, brokerAdress, 5556);

    // Setup bluetooth serial connection
    btc::bluetooth_controller bt(device);
    boost::asio::serial_port port = bt.connect();

    while (!stop) {
        char c;
        auto *buffer = new char[BUFFER_SIZE];
        int i = 0;

        // Read commands from serial
        while (boost::asio::read(port, boost::asio::buffer(&c, 1)) == 1) {
            buffer[i++] = c;

            if (c == '}') {
                break; // End of command
            }
        }

        // Find begin, separator and end of message
        std::string command(buffer);
        std::size_t begin_pos = command.find('{');
        std::size_t separator_pos = command.find(':');
        std::size_t end_pos = command.find('}');

        // Put data in Control struct
        Control control(
                CONTROL_MAP[command.substr(begin_pos + 1, separator_pos - begin_pos - 1)],      // Substring between '{' and ':'
                command.substr(separator_pos + 1, end_pos - separator_pos - 1)); // Substring between ':' and '}'

        // Convert to protobuf Message
        Message message = convert(control);

        // Print controller input
        if(debug){
            std::cout << std::endl << "control: " << control.control << std::endl << "state: " << control.value << std::endl << std::endl;
        }

        // Send topic to broker
        pub.publish(message);
    }
}
