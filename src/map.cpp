#include "map.h"

void initConfig() {
    // Map function to controller input
    CONFIGURATION.emplace(MODE, 0);
    CONFIGURATION.emplace(SENSITIVITY, 255);
}

int getConfig(CONFIG config) {
    return CONFIGURATION[config];
}

void setConfig(CONFIG config, int value) {
    CONFIGURATION[config] = value;
}
