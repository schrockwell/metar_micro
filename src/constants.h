#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Arduino.h>
#include <NeoPixelBus.h>

namespace Debug
{
    const bool PRINT_METARS = true;
}

namespace Pins
{
    const uint STATUS_LED = 0;
    const uint NEOPIXEL = 16;
    const uint BRIGHTNESS_POT = A1;
    const uint CONTRAST_POT = A0;
    const uint LDR = A2;

    const uint DIP_WIND = 5;
    const uint DIP_LIGHTNING = 6;
    const uint DIP_DIMMING = 7;
    const uint DIP_WIFI_SETUP = 8;
}

namespace Colors
{
    const uint8_t BRIGHTNESS = 255; // out of 255
    const uint8_t LIGHTNING_BRIGHTNESS = 255;

    const RgbColor RED = RgbColor(BRIGHTNESS, 0, 0);
    const RgbColor GREEN = RgbColor(0, BRIGHTNESS, 0);
    const RgbColor BLUE = RgbColor(0, 0, BRIGHTNESS);
    const RgbColor MAGENTA = RgbColor(BRIGHTNESS, 0, BRIGHTNESS);
    const RgbColor YELLOW = RgbColor(BRIGHTNESS, BRIGHTNESS / 2, 0);
    const RgbColor BLACK = RgbColor(0);

    const RgbColor VFR = GREEN;
    const RgbColor MVFR = BLUE;
    const RgbColor IFR = RED;
    const RgbColor LIFR = MAGENTA;

    const RgbColor LIGHTNING_FLASH = RgbColor(LIGHTNING_BRIGHTNESS);

    const RgbColor INITIALIZING = GREEN;
    const RgbColor CONNECTED_NO_DATA = YELLOW;
    const RgbColor DISCONNECTED = RED;
}

namespace Config
{
    // Animation
    const uint ANIMATION_FRAME_DURATION = 100; // ms
    const uint GUSTY_WIND_THRESHOLD = 15;      // kt
    const uint FLICKER_WIND_PERCENT = 20;      // %
    const uint FLICKER_WIND_DIMMING = 128;     // 255 = original, 0 = black
    const uint FLASH_LIGHTNING_PERCENT = 5;    // %

    // LEDs
    const uint MIN_BRIGHTNESS = 5; // out of 255

    // Timing
    const uint METAR_FETCH_INTERVAL = 300000; // 5 minutes (after successful fetch)
    const uint METAR_RETRY_INTERVAL = 60000;  // 1 minute (after failed fetch)
    const uint WIFI_RETRY_INTERVAL = 0;       // immediately (after failed connect)
}

#endif