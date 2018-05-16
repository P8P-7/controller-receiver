#pragma once

enum CONFIG {
    MODE = 0,
    SENSITIVITY = 1,
};

void setConfig(CONFIG key, int value);
