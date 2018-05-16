#pragma once

#include <string>

enum CONFIG {
    MODE = 0,
    SENSITIVITY = 1,
};

CONFIG stringToConfig(std::string string);
