#ifndef INPUTS_H
#define INPUTS_H

#include "types.h"

namespace Inputs
{
    const int DEBOUNCE_DELAY = 100;    // ms
    const int LONG_PRESS_DELAY = 2000; // ms

    void setup();
    inputs_t read();
    void debounce(int delay, bool value, bool &prevValue, unsigned long &prevMillis);
}

#endif