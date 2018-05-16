#include "include/goliath/bluetooth_controller.h"

using namespace goliath::btc;

BluetoothController::BluetoothController(const char* dev){
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

    // Find begin, separator and end of message
    std::string command(buffer);
    std::size_t begin_pos = command.find('{');
    std::size_t type_separator_pos = command.find(';');
    std::size_t separator_pos = command.find(':');
    std::size_t end_pos = command.find('}');

//    std::string type = "1";
    if(begin_pos > 0 && type_separator_pos > begin_pos && separator_pos > type_separator_pos && end_pos > separator_pos) {
        std::string type = command.substr(begin_pos + 1, type_separator_pos - begin_pos - 1);
        std::string key = command.substr(type_separator_pos + 1, separator_pos - type_separator_pos - 1);
        std::string value = command.substr(separator_pos + 1, end_pos - separator_pos - 1);
        return std::make_tuple(type,key,value);
    }
    return std::make_tuple("-1","-1","-1");
}

