#include <goliath/bluetooth_controller.h>

using namespace goliath::btc;

BluetoothController::BluetoothController(const char *dev) {
    serialPort.open(dev);
    serialPort.set_option(boost::asio::serial_port_base::baud_rate(9600)); // Default for bluetooth
    serialPort.set_option(boost::asio::serial_port_base::character_size(8));
}

std::tuple<std::string, std::string, std::string> BluetoothController::receive() {
    char c;
    auto *buffer = new char[BUFFER_SIZE];
    int i = 0;

    // Read commands from serial
    while (boost::asio::read(serialPort, boost::asio::buffer(&c, 1)) == 1) {
        buffer[i++] = c;

        if (c == '}') {
            break; // End of command
        }
    }

    return convertInput(buffer);
}

void BluetoothController::send(STATUS status, int value) {
    statusMessage statusMessage(status, value);
    const char * message = reinterpret_cast<const char *>(&statusMessage);
    boost::asio::write(serialPort, boost::asio::buffer(message, strlen(message)));
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

    BOOST_LOG_TRIVIAL(warning) << "Invalid message \"" << command.substr(begin_pos, end_pos + 1) << "\" received.";

    return std::make_tuple("-1", "-1", "-1");
}

statusMessage::statusMessage(STATUS stat, int val) {
    status = stat;
    value = val;
}
