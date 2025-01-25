#ifndef LEDS_H
#define LEDS_H

#include <NeoPixelBus.h>

#include "types.h"

namespace LEDs
{
    // Setup
    void setupStrip();

    // Drawing loop
    void loopRedraw();
    void drawMETARs();
    void drawAnimationFrame();
    void drawStatus(bool flash);

    // Getters
    uint8_t getDesiredBrightness();
    RgbColor getCategoryColor(const category_t category);

    // Setters
    void clearStrip();
    void setStationPixel(int index, RgbColor color);
    void setStatus(status_t status);
    void setStatusPixel(RgbColor color);
    void previewBrightness(int brightness);
}

#endif