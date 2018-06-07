#include <zmq.hpp>
#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/log/trivial.hpp>
#include <goliath/bluetooth_controller.h>
#include <goliath/zmq-messaging.h>
#include <goliath/foundation.h>
#include <MessageCarrier.pb.h>

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
 * @fn void initControls()
 * @brief Initializes receiver mode.
 */
void initControls() {
    if (!FUNCTION_MAP.empty()) {
        FUNCTION_MAP.clear();
        BOOST_LOG_TRIVIAL(debug) << "Changed to mode " << getConfig(MODE);
    }
    // Map function to controller input
    switch (getConfig(MODE)) {
        default:
        case 0:
            FUNCTION_MAP.emplace(JSLX, dualJoystickToMove);
            FUNCTION_MAP.emplace(JSLY, dualJoystickToMove);
            FUNCTION_MAP.emplace(JSRX, dualJoystickToMove);
            FUNCTION_MAP.emplace(JSRY, dualJoystickToMove);
            FUNCTION_MAP.emplace(BTN1, buttonToFrontWing);
            FUNCTION_MAP.emplace(BTN2, buttonToFrontWing);
            FUNCTION_MAP.emplace(BTN3, buttonToFrontWing);
            FUNCTION_MAP.emplace(BTN4, buttonToFrontWing);
            break;
        case 1:
            FUNCTION_MAP.emplace(JSLX, dualJoystickToMove);
            FUNCTION_MAP.emplace(JSLY, dualJoystickToMove);
            FUNCTION_MAP.emplace(JSRX, dualJoystickToMove);
            FUNCTION_MAP.emplace(JSRY, dualJoystickToMove);
            FUNCTION_MAP.emplace(BTN1, buttonToBackWing);
            FUNCTION_MAP.emplace(BTN2, buttonToBackWing);
            FUNCTION_MAP.emplace(BTN3, buttonToBackWing);
            FUNCTION_MAP.emplace(BTN4, buttonToBackWing);
            break;
        case 2:
            FUNCTION_MAP.emplace(JSLX, dualJoystickToMove);
            FUNCTION_MAP.emplace(JSLY, dualJoystickToMove);
            FUNCTION_MAP.emplace(JSRX, dualJoystickToMove);
            FUNCTION_MAP.emplace(JSRY, dualJoystickToMove);
            FUNCTION_MAP.emplace(BTN1, buttonToBackWing);
            FUNCTION_MAP.emplace(BTN2, buttonToBackWing);
            FUNCTION_MAP.emplace(BTN3, buttonToBackWing);
            FUNCTION_MAP.emplace(BTN4, buttonToBackWing);
            break;
    }
}

/**
 * @fn void sendToController()
 * @brief Sends incoming messages to controller.
 */
void sendToController(proto::MessageCarrier messageCarrier) {
    //TODO convert repo message into data for controller
    BOOST_LOG_TRIVIAL(debug) << "Sent message to conrtoller.";
}

/**
 * @fn void show_usage(std::string name)
 * @brief print help menu to console
 * @param name executable name
 */
static void show_usage(std::string name) {
    std::cerr << "Usage: " << name << " <option(s)> ARGUMENT\n"
              << "Options:\n"
              << "\t-h,--help\t\tShow this help message\n"
              << "\t-b,--broker IP\t\tSpecify the broker ip-address\n"
              << "\t-d,--device DEVICE\tSpecify the device path\n"
              << "\t-a,-- ADDRESS\tSpecify the device hardware address\n";
}

/**
 * @fn int main(int argc, char **argv)
 * @brief main function
 * @param argc number of arguments
 * @param argv arguments
 */
int main(int argc, char **argv) {
    boost::log::trivial::severity_level logLevel = boost::log::trivial::info;
    const int BUFFER_SIZE = 128;
    const char *brokerAdress = "127.0.0.1";
    const char *devicePath = "/dev/rfcomm0";
    std::string devicAddress = "98:D3:31:FD:15:48";
    bool stop = false;

    // Command line options
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "-h") || (arg == "--help")) {
            show_usage(argv[0]);
            return 0;
        }
        if ((arg == "-b") || (arg == "--broker")) {
            if (i + 1 < argc) {
                i++;
                brokerAdress = argv[i];
            } else {
                std::cerr << "--broker option requires one argument." << std::endl;
                return 1;
            }
        } else if ((arg == "-d") || (arg == "--device")) {
            if (i + 1 < argc) {
                i++;
                devicePath = argv[i];
            } else {
                std::cerr << "--device option requires one argument." << std::endl;
                return 1;
            }
        } else if ((arg == "-a") || (arg == "--address")) {
            if (i + 1 < argc) {
                i++;
                devicAddress = argv[i];
            } else {
                std::cerr << "--address option requires one argument." << std::endl;
                return 1;
            }
        } else if (arg == "--debug") {
            logLevel = boost::log::trivial::debug;
        }
    }

    util::Console console(&util::colorConsoleFormatter,
                          argv[0],
                          "converter-text.txt",
                          logLevel);

    BOOST_LOG_TRIVIAL(info) << "Starting Controller Converter.";

    initConfig();
    initControls();

    // Setup bluetooth serial connection
    btc::BluetoothController bt(devicePath, devicAddress);
    bt.clear();
    bt.send(btc::Status::BT_CONNECTED, 0);

    // Setup ZMQ publisher and subscriber
    zmq::context_t context(2);
    goliath::messaging::ZmqPublisher pub(context, brokerAdress, 5556);
    goliath::messaging::ZmqSubscriber sub(context, brokerAdress, 5556);

    sub.bind(proto::MessageCarrier::kSynchronizeMessage, sendToController);


    while (!stop) {
        // Wait for input data from controller
        std::tuple<std::string, std::string, std::string> input = bt.receive();

        // Get input type
        TYPE type = stringToType(std::get<0>(input));

        switch (type) {
            case CONTROL_TYPE: {
                // Convert to protobuf Message
                CONTROL control = stringToControl(std::get<1>(input));
                int value = stringToValue(std::get<2>(input));
                proto::MessageCarrier message;
                if (control < CONTROL_NR_ITEMS) {
                    message = convertControl(control, value, FUNCTION_MAP);
                } else {
                    bt.send(btc::Status::BT_INVALID_INPUT, 0);
                }
                // Send topic to broker
                if (message.ByteSize() > 0) {
                    pub.publish(message);
                } else {
                    BOOST_LOG_TRIVIAL(warning) << "Received invalid controller input.";
                }
                break;
            }
            case CONFIG_TYPE: {
                // Convert string to CONFIG
                CONFIG config = stringToConfig(std::get<1>(input));
                int value = stringToValue(std::get<2>(input));

                // Set config
                setConfig(config, value);

                // Reset controls when mode changes
                if (config == MODE) {
                    initControls();
                }
                BOOST_LOG_TRIVIAL(debug) << "Received config \"" << config << "\" with value \"" << value
                                         << "\" from controller.";
                break;
            }
            case LASTSTATUS_TYPE: {
                bt.sendLast();
                BOOST_LOG_TRIVIAL(debug) << "Sent last status to controller";
                break;
            }
            case CONNECTIONLOST_TYPE: {
                BOOST_LOG_TRIVIAL(error) << "Connection to controller lost.";
                bt.reconnect();
                bt.clear();
                break;
            }
            case IGNORE_TYPE: {
                break;
            }
            default:
                bt.send(btc::Status::BT_INVALID_INPUT, 0);
                BOOST_LOG_TRIVIAL(warning) << "Received invalid message from controller";
                break;
        }
    }
}
