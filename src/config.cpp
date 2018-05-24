#include "config.h"

CONFIG stringToConfig(std::string string) {
    return static_cast<CONFIG>(std::stoi(string.c_str()));
}
