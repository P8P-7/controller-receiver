#include <zmq.hpp>
#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/log/trivial.hpp>
#include <goliath/bluetooth_controller.h>
#include <goliath/zmq-messaging.h>
#include <goliath/foundation.h>
#include <MessageCarrier.pb.h>
#include <SynchronizeMessage.pb.h>
#include <repositories/BatteryRepository.pb.h>
#include <repositories/LogRepository.pb.h>
#include <boost/algorithm/string.hpp>

#include "config.h"
#include "control.h"
#include "map.h"
#include "convert.h"


/**
 * @file main.cpp
 * @author Group 7 - Informatica
 */
using namespace goliath;

btc::BluetoothController bt;

/**
 * @fn void initControls()
 * @brief Initializes receiver mode.
 */
void initControls() {
    if (!FUNCTION_MAP.empty()) {
        FUNCTION_MAP.clear();
        BOOST_LOG_TRIVIAL(info) << "Changed to mode " << getConfig(MODE);
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
            FUNCTION_MAP.emplace(BTN1, buttonToAllWing);
            FUNCTION_MAP.emplace(BTN2, buttonToAllWing);
            FUNCTION_MAP.emplace(BTN3, buttonToAllWing);
            FUNCTION_MAP.emplace(BTN4, buttonToAllWing);
            break;
        case 3:
            FUNCTION_MAP.emplace(JSLX, nullptr);
            FUNCTION_MAP.emplace(JSLY, nullptr);
            FUNCTION_MAP.emplace(JSRX, nullptr);
            FUNCTION_MAP.emplace(JSRY, nullptr);
            FUNCTION_MAP.emplace(BTN1, nullptr);
            FUNCTION_MAP.emplace(BTN2, nullptr);
            FUNCTION_MAP.emplace(BTN3, nullptr);
            FUNCTION_MAP.emplace(BTN4, nullptr);
            break;
    }
}

/**
 * @fn void sendToController(const proto::MessageCarrier &messageCarrier)
 * @brief Sends incoming messages to controller.
 * @param messageCarrier Message from subscriber
 */
void sendToController(const proto::MessageCarrier &messageCarrier) {
    if (!messageCarrier.has_synchronizemessage()) {
        BOOST_LOG_TRIVIAL(error) << "Incoming message does not have synchronized message.";
        return;
    }

    const proto::SynchronizeMessage &synchronizeMessage = messageCarrier.synchronizemessage();

    for (const auto &anyMessage : synchronizeMessage.messages()) {
        if (anyMessage.Is<proto::repositories::BatteryRepository>()) {
            proto::repositories::BatteryRepository batteryRepository;
            anyMessage.UnpackTo(&batteryRepository);

            int32_t level = batteryRepository.level();
            bt.send(btc::Status::BT_BATTERY, level);

            BOOST_LOG_TRIVIAL(info) << "Sent battery level " << level << " to controller.";
        } else if (anyMessage.Is<proto::repositories::LogRepository>()) {
            proto::repositories::LogRepository logRepository;
            anyMessage.UnpackTo(&logRepository);

            int count = 0;

            for (const proto::repositories::LogRepository_Entry &logRepositoryEntry : logRepository.entries()) {

                proto::repositories::LogSeverity severity = logRepositoryEntry.severity();

                if (severity >= proto::repositories::LogSeverity::WARNING) {
                    std::string repoMessage = logRepositoryEntry.message();
                    boost::replace_all(repoMessage, "\"", "\'");
                    bt.send(severity, repoMessage);
                    BOOST_LOG_TRIVIAL(info) << "Sent error log to controller.";
                    count++;
                }
                if (count > 0) {
                    break;
                }
            }
        }
    }

    BOOST_LOG_TRIVIAL(debug) << "Sent message to conrtoller.";
}

/**
 * @fn void show_usage(std::string name)
 * @brief print help menu to console
 * @param name executable name
 */
static void show_usage(const std::string &name) {
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

    util::Console console(&util::colorConsoleFormatter, argv[0], "converter-text.txt", logLevel);

    if (geteuid() != 0) {
        BOOST_LOG_TRIVIAL(error) << "Root privileges needed.";
        return 1;
    }

    BOOST_LOG_TRIVIAL(info) << "Starting Controller Converter.";

    initConfig();
    initControls();

    // Setup bluetooth serial connection
    bt.start(devicePath, devicAddress);
    bt.clear();
    bt.send(btc::Status::BT_CONNECTED, 0);

    // Setup ZMQ publisher and subscriber
    zmq::context_t context(2);
    goliath::messaging::ZmqPublisher pub(context, brokerAdress, 5556);
    goliath::messaging::ZmqSubscriber sub(context, brokerAdress, 5555);

    sub.bind(proto::MessageCarrier::kSynchronizeMessage, sendToController);
    sub.start();

    // Setup invalidate message to request all repo data on (re)connect
    MessageCarrier invalidateMessage;
    auto *commandMessage(new CommandMessage);
    auto *invalidateCommand(new commands::InvalidateAllCommand);
    commandMessage->set_allocated_invalidateallcommand(invalidateCommand);
    invalidateMessage.set_allocated_commandmessage(commandMessage);

    // Request all repo data
    pub.publish(invalidateMessage);
    BOOST_LOG_TRIVIAL(info) << "Send invalidate command to core.";

    while (!stop) {
        // Wait for input data from controller
        btc::Input input = bt.receive();

        // If input is correctly received
        if (input.error == btc::InputError::IE_SUCCES) {
            // Get input type
            TYPE type = stringToType(input.type);

            switch (type) {
                case CONTROL_TYPE: {
                    // Convert to protobuf Message
                    CONTROL control = stringToControl(input.control);
                    int value = stringToValue(input.value);
                    proto::MessageCarrier message;
                    if (control < CONTROL_NR_ITEMS) {
                        message = convertControl(control, value, FUNCTION_MAP);
                    }
                    // Send topic to broker if message no empty
                    if (message.ByteSize() > 0) {
                        pub.publish(message);
                    } else {
                        BOOST_LOG_TRIVIAL(warning) << "Received invalid controller input.";
                    }
                    break;
                }
                case CONFIG_TYPE: {
                    // Convert string to CONFIG
                    CONFIG config = stringToConfig(input.control);
                    int value = stringToValue(input.value);

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
                case COMMAND_TYPE: {
                    // Convert to protobuf Message
                    CommandMessage::CommandCase command = stringToCommandCase(input.control);
                    proto::MessageCarrier message;
                    message = inputToCommand(command);

                    // Send topic to broker
                    if (message.ByteSize() > 0) {
                        pub.publish(message);
                    }
                    break;
                }
                case LAST_STATUS_TYPE: {
                    // Send last status if it was not received correctly
                    bt.sendLast();
                    BOOST_LOG_TRIVIAL(debug) << "Sent last status to controller";
                    break;
                }
                default:
                    break;
            }
        } else if (input.error == btc::InputError::IE_CONNECTION_LOST) {
            BOOST_LOG_TRIVIAL(error) << "Connection to controller lost.";
            bt.reconnect();

            pub.publish(invalidateMessage);

            BOOST_LOG_TRIVIAL(info) << "Send invalidate command to core.";

        } else if (input.error == btc::InputError::IE_WRONG_FORMAT || input.error == btc::InputError::IE_WRONG_VALUE) {
            bt.send(btc::Status::BT_INVALID_INPUT, 0);
            BOOST_LOG_TRIVIAL(warning) << "Received invalid message from controller";
        }
    }
    return 0;
}
