#include "control.h"

CONTROL stringToControl(std::string string) {
    return static_cast<CONTROL>(atoi(string.c_str()));
}

int stringToValue(std::string string) {
    return std::stoi(string.c_str());
}
