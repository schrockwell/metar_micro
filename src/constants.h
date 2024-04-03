#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <NeoPixelBus.h>

namespace Pins
{
    const uint STATUS_LED = 0;
    const uint NEOPIXEL = 16;
}

namespace Colors
{
    const uint8_t BRIGHTNESS = 32; // out of 255

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

#endif