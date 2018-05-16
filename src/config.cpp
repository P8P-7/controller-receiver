#include "config.h"

CONFIG stringToConfig(std::string string) {
    return static_cast<CONFIG>(atoi(string.c_str()));
}
