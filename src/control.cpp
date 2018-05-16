#include "control.h"

CONTROL stringToControl(std::string string) {
    return static_cast<CONTROL>(atoi(string.c_str()));
}

int stringToValue(std::string string) {
    return atoi(string.c_str());
}
