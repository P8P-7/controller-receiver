#include "main.h"


int main() {
    const int BUFFER_SIZE = 1024;

    bool stop = false;

    const char *brokerAdress = "localhost";
    zmq::context_t context(1);
    io::zmq_publisher pub(context, brokerAdress, 5556);

    const char *device = "/dev/rfcomm1";
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

        Control control(command.substr(begin_pos+1, separator_pos - begin_pos - 1),      // Substring between '{' and ':'
                        command.substr(separator_pos + 1, end_pos - separator_pos - 1)); // Substring between ':' and '}'



//        std::cout << "command: " << command << std::endl << "control: " << control << std::endl << "state: " << state << std::endl << std::endl;


//         Send topic
//         pub.publish()
    }
}
