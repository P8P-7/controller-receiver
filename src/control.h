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

struct Control {
    CONTROL control;
    int value;
public:
    Control(CONTROL con, int val) {
        control = con;
        value = val;
    }
    Control(std::string con, std::string val) {
        control = static_cast<CONTROL>(atoi(con.c_str()));
        value = atoi(val.c_str());
    }
};
