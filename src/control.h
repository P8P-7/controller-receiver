#pragma once

#include <string>

enum CONTROL {
    JSLX = 0,
    JSLY = 1,
    JSRX = 2,
    JSRY = 3,
    BTN1 = 4,
    BTN2 = 5,
    BTN3 = 6,
    BTN4 = 7,
};

CONTROL stringToControl(std::string string);

int stringToValue(std::string string);
