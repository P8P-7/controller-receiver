#include <goliath/bluetooth_controller.h>

using namespace goliath::btc;

BluetoothController::BluetoothController(const std::string &newDevicePath, std::string &newDeviceAddress) {
    devicePath = newDevicePath.c_str();
    deviceAddress = newDeviceAddress.c_str();

    BOOST_LOG_TRIVIAL(info) << "Connecting to " << deviceAddress.c_str() << " on " << devicePath;

    while (!connect()) {
        BOOST_LOG_TRIVIAL(error) << "Could not connect to controller, retrying...";
    };

    BOOST_LOG_TRIVIAL(info) << "Connected to controller.";

    serialPort.set_option(boost::asio::serial_port_base::baud_rate(9600)); // Default for bluetooth
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
            if(!serialPort.is_open()) {
                serialPort.open(devicePath);
            }
            return true;
        }
        sleep(2);
    }

    return false;
}

void BluetoothController::reconnect() {
    if(serialPort.is_open()){
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
        serialPort.set_option(boost::asio::serial_port_base::baud_rate(9600)); // Default for bluetooth
        serialPort.set_option(boost::asio::serial_port_base::character_size(8));
    }

    const boost::system::error_code ec;

    boost::asio::write(serialPort, boost::asio::buffer("write_test"));

    serialPort.close();

    if (ec) {
        return false;
    }
    return true;
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

void BluetoothController::send(Status status, short value) {
    boost::system::error_code error;

    try {
        lastMessage.set(status, value);

        std::ostringstream oss;
        oss << "{" << static_cast<int>(status) << ":" << value << "}";
        std::string message = oss.str();

        boost::asio::write(serialPort, boost::asio::buffer(message), error);

        if (error) {
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

            if (error) {
                BOOST_LOG_TRIVIAL(error) << error.message().c_str();
                return;
            }
        } catch (const boost::system::system_error &ex) {
            BOOST_LOG_TRIVIAL(error) << "Could not send message to controller.";
        }
    }
}

std::tuple<std::string, std::string, std::string> BluetoothController::convertInput(char *buffer) {
    std::string command(buffer);
    std::size_t begin_pos = command.find('{');
    std::size_t type_separator_pos = command.find(';');
    std::size_t separator_pos = command.find(':');
    std::size_t end_pos = command.find('}');

    if (type_separator_pos - begin_pos > 1 && separator_pos - type_separator_pos > 1 && end_pos - separator_pos > 1 &&
        begin_pos != std::string::npos && type_separator_pos != std::string::npos &&
        separator_pos != std::string::npos && end_pos != std::string::npos && end_pos - begin_pos > 5) {
        std::string type = command.substr(begin_pos + 1, type_separator_pos - begin_pos - 1);
        std::string key = command.substr(type_separator_pos + 1, separator_pos - type_separator_pos - 1);
        std::string value = command.substr(separator_pos + 1, end_pos);
        return std::make_tuple(type, key, value);
    }

    BOOST_LOG_TRIVIAL(warning) << "Invalid message \"" << command.substr(begin_pos, end_pos + 1).c_str()
                               << "\" received.";

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



