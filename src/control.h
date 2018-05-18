#pragma once

#include <string>

enum CONTROL {
    JSLX, JSLY, JSRX, JSRY, BTN1, BTN2, BTN3, BTN4, CONTROL_NR_ITEMS
};

CONTROL stringToControl(std::string string);

int stringToValue(std::string string);
