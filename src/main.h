#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>

#include "types.h"

void setup();
void loop();

namespace Main
{
    void setupStrip();

    void loopMETARFetch();
    void loopStatusLED();

    void displayMETARs(String lines);
    void displayMETAR(String metar);
    void setStatusLED(status_t newStatus);
    void clearStrip();
}

#endif