#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>

#include "types.h"

void setup();
void loop();

namespace Main
{
    void setupMETARs();
    void setupStrip();

    void loopMETARFetch();
    void loopStatusLED();
    void loopAnimate();

    void displayMETARs();
    void setStatusLED(status_t newStatus);
    void clearStrip();

    RgbColor getCategoryColor(const category_t category);
}

#endif