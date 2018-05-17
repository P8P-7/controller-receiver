#include <zmq.hpp>
#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <goliath/bluetooth_controller.h>
#include <goliath/zmq_publisher.h>

#include "config.h"
#include "control.h"
#include "map.h"
#include "convert.h"

/**
 * @file main.cpp
 * @author Group 7 - Informatica
 */

using namespace goliath;

/**
 * @fn void initConfig()
 * @brief Initializes controller configuration.
 */
void initConfig() {
    // Map function to controller input
    CONFIGURATION.emplace(MODE, 0);
    CONFIGURATION.emplace(SENSITIVITY, 255);
}

/**
 * @fn void initControls()
 * @brief Initializes receiver mode.
 */
void initControls() {
    if (FUNCTION_MAP.size() != 0) {
        FUNCTION_MAP.clear();
    }
    // Map function to controller input
    switch (CONFIGURATION.find(MODE)->second) {
        default:
        case 0:
            FUNCTION_MAP.emplace(JSLX, dualJoystickToMove);
            FUNCTION_MAP.emplace(JSLY, dualJoystickToMove);
            FUNCTION_MAP.emplace(JSRX, dualJoystickToMove);
            FUNCTION_MAP.emplace(JSRY, dualJoystickToMove);
            FUNCTION_MAP.emplace(BTN1, buttonToMessage);
            FUNCTION_MAP.emplace(BTN2, buttonToMessage);
            FUNCTION_MAP.emplace(BTN3, buttonToMessage);
            FUNCTION_MAP.emplace(BTN4, buttonToMessage);
            break;
    }
}

/**
 * @fn void show_usage(std::string name)
 * @brief print help menu to console
 * @param executable name
 */
static void show_usage(std::string name) {
    std::cerr << "Usage: " << name << " <option(s)> ARGUMENT\n"
              << "Options:\n"
              << "\t-h,--help\t\tShow this help message\n"
              << "\t-a,--address IP\t\tSpecify the broker ip-address\n"
              << "\t-d,--device DEVICE\tSpecify the device path\n";
}

/**
 * @fn int main(int argc, char **argv)
 * @brief main function
 * @param number of arguments
 * @param arguments
 */
int main(int argc, char **argv) {
    const int BUFFER_SIZE = 1024;
    const char *brokerAdress = "127.0.0.1";
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
        if ((arg == "-a") || (arg == "--address")) {
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

    initConfig();
    initControls();

    // Setup ZMQ publisher
    zmq::context_t context(1);
    io::zmq_publisher pub(context, brokerAdress, 5556);

    // Setup bluetooth serial connection
    btc::BluetoothController bt(device);

    while (!stop) {
        // Wait for input data from controller
        std::tuple<std::string, std::string, std::string> input = bt.receive(debug);

        // Get input type
        TYPE type = stringToType(std::get<0>(input));

        switch (type) {
            case CONTROL_TYPE: {
                // Convert to protobuf Message
                CONTROL control = stringToControl(std::get<1>(input));
                int value = stringToValue(std::get<2>(input));
                Message message = convertControl(control, value, FUNCTION_MAP);
                // Send topic to broker
                if(message.ByteSize() > 0){
                    pub.publish(message);
                }

                if (debug) {
                    std::cerr << std::endl << "control: " << control << std::endl << "state: "
                              << static_cast<int>((float) value * ((float) CONFIGURATION[SENSITIVITY] / 255.0))
                              << std::endl;
                }
                break;
            }
            case CONFIG_TYPE: {
                // Convert string to CONFIG
                CONFIG config = stringToConfig(std::get<1>(input));
                int value = stringToValue(std::get<2>(input));

                // Set config
                CONFIGURATION[config] = value;

                // Reset controls when mode changes
                if (config == MODE) {
                    initControls();
                }

                if (debug) {
                    std::cerr << std::endl << "config: " << config << std::endl << "state: " << value << std::endl;
                }
                break;
            }
            default:
                if (debug) {
                    std::cerr << "unknown input\n";
                }
                break;
        }
    }
}
