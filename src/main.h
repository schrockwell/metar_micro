#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <NeoPixelBus.h>

#include "types.h"

void setup();
void loop();

namespace Main
{
    // Setup
    void setupSettings();
    void setupMETARs();
    void setupStrip();

    // Loop
    void loopMETARFetch();
    void loopRedraw();
    void loopInputs();
    void loopWifiSetup();

    // Drawing loop
    void drawMETARs();
    void drawAnimationFrame();
    void drawStatus(bool flash);

    // LEDs
    uint8_t getDesiredBrightness();
    void setStatus(status_t status);
    void clearStrip();
    RgbColor getCategoryColor(const category_t category);
    void setStationPixel(int index, RgbColor color);
    void setStatusPixel(RgbColor color);
    void previewBrightness(int brightness);

    // Debugging
    void printMetars();
}

#endif