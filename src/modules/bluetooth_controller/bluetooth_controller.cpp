#include <goliath/bluetooth_controller.h>

using namespace goliath::btc;

BluetoothController::BluetoothController(const std::string &newDevicePath, std::string &newDeviceAddress) {
    devicePath = newDevicePath.c_str();
    deviceAddress = newDeviceAddress;

    BOOST_LOG_TRIVIAL(info) << "Connecting to " << deviceAddress.c_str() << " on " << devicePath;

    while (!connect()) {
        BOOST_LOG_TRIVIAL(error) << "Could not connect to controller, retrying...";
    };

    BOOST_LOG_TRIVIAL(info) << "Connected to controller.";

    serialPort.set_option(boost::asio::serial_port_base::baud_rate(38400));
    serialPort.set_option(boost::asio::serial_port_base::character_size(8));
}

bool BluetoothController::connect() {
    int nTries = 5;

    std::ostringstream oss;
    oss << "rfcomm connect hci0 " << deviceAddress << " > /dev/null 2>&1 &";
    std::string cmd = oss.str();

    BOOST_LOG_TRIVIAL(debug) << "\"" << cmd.c_str() << "\" executed";

    system("pkill rfcomm");
    system(cmd.c_str());

    for (int i = 0; i < nTries; i++) {
        if (connected()) {
            if (!serialPort.is_open()) {
                serialPort.open(devicePath);
            }
            return true;
        }
        sleep(2);
    }

    return false;
}

void BluetoothController::reconnect() {
    if (serialPort.is_open()) {
        serialPort.close();
    }

    BOOST_LOG_TRIVIAL(info) << "Reconnecting to " << deviceAddress.c_str() << " on " << devicePath;

    while (!connect()) {
        BOOST_LOG_TRIVIAL(error) << "Could not reconnect to controller, retying...";
    };

    clear();

    BOOST_LOG_TRIVIAL(info) << "Reconnected to controller.";
}

bool BluetoothController::connected() {
    if (access(devicePath, W_OK) == -1) {
        return false;
    };

    if (!serialPort.is_open()) {
        serialPort.open(devicePath);
        serialPort.set_option(boost::asio::serial_port_base::baud_rate(38400)); // Default for bluetooth
        serialPort.set_option(boost::asio::serial_port_base::character_size(8));
    }

    const boost::system::error_code ec;

    boost::asio::write(serialPort, boost::asio::buffer("write_test"));

    serialPort.close();

    return ec == nullptr;
}

std::tuple<std::string, std::string, std::string> BluetoothController::receive() {
    char c;
    auto *buffer = new char[BUFFER_SIZE];
    int i = 0;

    try {
        // Read commands from serial
        while (boost::asio::read(serialPort, boost::asio::buffer(&c, 1)) == 1) {
            buffer[i++] = c;

            if (c == '}') {
                break; // End of command
            }
        }
    } catch (const boost::system::system_error &ex) {
        return std::make_tuple("-2", "-2", "-2");
    }

    return convertInput(buffer);
}

void BluetoothController::send(Status status, int value) {
    boost::system::error_code error;

    try {
        lastMessage.set(status, value);

        std::ostringstream oss;
        oss << "{" << static_cast<int>(status) << ":" << value << "}";
        std::string message = oss.str();

        boost::asio::write(serialPort, boost::asio::buffer(message), error);

        if (error != nullptr) {
            BOOST_LOG_TRIVIAL(error) << error.message().c_str();
            return;
        }

        BOOST_LOG_TRIVIAL(debug) << "Sent status message \"" << message.c_str() << "\" to controller.";

    } catch (const boost::system::system_error &ex) {
        BOOST_LOG_TRIVIAL(error) << "Could not send message to controller.";
    }
}

void BluetoothController::sendLast() {
    if (lastMessage.isSet) {
        boost::system::error_code error;

        try {
            std::ostringstream oss;
            oss << "{" << static_cast<int>(lastMessage.status) << ":" << lastMessage.value << "}";
            std::string message = oss.str();

            boost::asio::write(serialPort, boost::asio::buffer(message), error);

            if (error != nullptr) {
                BOOST_LOG_TRIVIAL(error) << error.message().c_str();
                return;
            }
        } catch (const boost::system::system_error &ex) {
            BOOST_LOG_TRIVIAL(error) << "Could not send message to controller.";
        }
    }
}

std::tuple<std::string, std::string, std::string> BluetoothController::convertInput(char *buffer) {
    BOOST_LOG_TRIVIAL(debug) << "buffer: " << buffer;
    if (buffer != 0) {
        std::string command(buffer);
        std::vector<std::size_t> pos;
        pos.emplace_back(command.find('{'));
        pos.emplace_back(command.find(';'));
        pos.emplace_back(command.find(':'));
        pos.emplace_back(command.find('}'));

        if (pos[0] != std::string::npos && pos[1] != std::string::npos && pos[2] != std::string::npos &&
            pos[3] != std::string::npos && std::is_sorted(begin(pos), end(pos))) {
            std::string type = command.substr(pos[0] + 1, pos[1] - pos[0] - 1);
            std::string key = command.substr(pos[1] + 1, pos[2] - pos[1] - 1);
            std::string value = command.substr(pos[2] + 1, pos[3]);
            return std::make_tuple(type, key, value);
        }
        BOOST_LOG_TRIVIAL(warning) << "Invalid message \"" << command.c_str() << "\" received.";
    } else {
        BOOST_LOG_TRIVIAL(warning) << "Empty message received.";
    }

    return std::make_tuple("-1", "-1", "-1");
}

void BluetoothController::clear() {
    if (0 == ::tcflush(serialPort.lowest_layer().native_handle(), TCIOFLUSH)) {
        boost::system::error_code();
    } else {
        boost::system::error_code(errno, boost::asio::error::get_system_category());
    }
    BOOST_LOG_TRIVIAL(info) << "Serial buffer flushed.";
}

StatusMessage::StatusMessage(Status stat, short val) {
    status = stat;
    value = val;
}

void StatusMessage::set(Status stat, short val) {
    status = stat;
    value = val;
    isSet = true;
}

StatusMessage::StatusMessage() {
    status = Status::BT_CONNECTED;
    value = 0;
}



