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
    void loopRedraw();
    void loopInputs();

    void drawMETARs();
    void drawAnimationFrame();
    void drawStatus();

    void clearStrip();

    RgbColor getCategoryColor(const category_t category);

    void setStationPixel(int index, RgbColor color);
}

#endif