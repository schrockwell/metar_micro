#ifndef INPUTS_H
#define INPUTS_H

#include "types.h"

namespace Inputs
{
    const int DEBOUNCE_DELAY = 100; // ms

    void setup();
    inputs_t read();
    void debounce(bool value, bool &prevValue, unsigned long &prevMillis);
}

#endif