#include <NeoPixelBus.h>

#ifndef CONSTANTS_H
#define CONSTANTS_H

const uint8_t BRIGHTNESS = 32; // out of 255

const uint STATUS_LED = 0;

RgbColor RED(BRIGHTNESS, 0, 0);
RgbColor GREEN(0, BRIGHTNESS, 0);
RgbColor BLUE(0, 0, BRIGHTNESS);
RgbColor MAGENTA(BRIGHTNESS, 0, BRIGHTNESS);
RgbColor BLACK(0);

const RgbColor VFR_COLOR = GREEN;
const RgbColor MVFR_COLOR = BLUE;
const RgbColor IFR_COLOR = RED;
const RgbColor LIFR_COLOR = MAGENTA;

const RgbColor INITIALIZING_COLOR = BLUE;
const RgbColor CONNECTED_NO_DATA_COLOR = MAGENTA;
const RgbColor DISCONNECTED_COLOR = RED;

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