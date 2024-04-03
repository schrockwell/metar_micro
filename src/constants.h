#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <NeoPixelBus.h>

const uint8_t BRIGHTNESS = 32; // out of 255

const uint STATUS_LED = 0;

namespace Colors
{
    const RgbColor RED = RgbColor(BRIGHTNESS, 0, 0);
    const RgbColor GREEN = RgbColor(0, BRIGHTNESS, 0);
    const RgbColor BLUE = RgbColor(0, 0, BRIGHTNESS);
    const RgbColor MAGENTA = RgbColor(BRIGHTNESS, 0, BRIGHTNESS);
    const RgbColor BLACK = RgbColor(0);

    const RgbColor VFR = GREEN;
    const RgbColor MVFR = BLUE;
    const RgbColor IFR = RED;
    const RgbColor LIFR = MAGENTA;

    const RgbColor INITIALIZING = BLUE;
    const RgbColor CONNECTED_NO_DATA = MAGENTA;
    const RgbColor DISCONNECTED = RED;
}

enum category_t
{
    NA,
    VFR,
    MVFR,
    IFR,
    LIFR,
};

enum status_t
{
    NEW,
    INITIALIZING,
    CONNECTED_NO_DATA,
    CONNECTED_WITH_DATA,
    DISCONNECTED
};

#endif