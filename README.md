# controller-receiver
[![Build Status](https://travis-ci.org/P8P-7/controller-receiver.svg?branch=master)](https://travis-ci.org/P8P-7/controller-receiver)

## Overview
_Goliath II_'s receiver for the controller. Receives input, converts it and sends it off to the broker to be received by the core.

## Build steps

### Raspbian (based on Debian 9 "Stretch")
```bash
git clone --recurse-submodules -j8 https://github.com/P8P-7/controller-receiver.git
cd controller-receiver
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
sudo make -j4
```
## Documentation
https://p8p-7.github.io/controller-receiver/index.html
