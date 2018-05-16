#include <zmq.hpp>
#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <goliath/bluetooth_controller.h>
#include <goliath/zmq_publisher.h>

#include "config.h"
#include "control.h"
#include "map.h"
#include "convert.h"

using namespace goliath;

void initConfig() {
    // Map function to controller input
    CONFIGURATION.emplace(MODE, 0);
    CONFIGURATION.emplace(SENSITIVITY, 255);
}

void initControls() {
    // Map function to controller input
    switch(CONFIGURATION.find(MODE)->second){
        default:
        case 0:
            FUNCTION_MAP.emplace(JSLX,joystickToMoveCommand);
            FUNCTION_MAP.emplace(JSLY,joystickToMoveCommand);
            FUNCTION_MAP.emplace(JSRX,joystickToMoveCommand);
            FUNCTION_MAP.emplace(JSRY,joystickToMoveCommand);
            FUNCTION_MAP.emplace(BTN1,buttonToMessage);
            FUNCTION_MAP.emplace(BTN2,buttonToMessage);
            FUNCTION_MAP.emplace(BTN3,buttonToMessage);
            FUNCTION_MAP.emplace(BTN4,buttonToMessage);
            break;
    }
}

static void show_usage(std::string name)
{
    std::cerr << "Usage: " << name << " <option(s)> ARGUMENT\n"
              << "Options:\n"
              << "\t-h,--help\t\tShow this help message\n"
              << "\t-a,--address IP\t\tSpecify the broker ip-address\n"
              << "\t-d,--device DEVICE\tSpecify the device path\n";
}

int main(int argc, char **argv) {
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

    // Set config
    initConfig();

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
        const int BUFFER_SIZE = 1024;
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

        std::string key = command.substr(begin_pos + 1, separator_pos - begin_pos - 1);
        std::string value = command.substr(separator_pos + 1, end_pos - separator_pos - 1);

        // Put data in Control struct
        Control control(
                key,            // Substring between '{' and ':'
                value           // Substring between ':' and '}'
        );

        // Print controller input
        if(debug){
            std::cout << std::endl << "control: " << control.control << std::endl << "state: " << control.value << std::endl << std::endl;
        }

        // Convert to protobuf Message
        Message message = convertControl(control, FUNCTION_MAP);

        // Send topic to broker
        pub.publish(message);
    }
}
