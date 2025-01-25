#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>

#include "board.h"
#include "types.h"

void setup();
void loop();

namespace Main
{
    inline system_t system;
    inline unsigned long _retryFetchAfter = 0;

    // Setup
    void setupSettings();
    void setupMETARs();

    // Loop
    void loopMETARFetch();
    void loopInputs();
    void loopWifiSetup();

    // Debugging
    void printMetars();

    // State management
    void setStatus(status_t status);

    // Setup
    void beginSetup();
    void endSetup();
}

#endif