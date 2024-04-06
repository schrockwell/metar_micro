#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Arduino.h>
#include <NeoPixelBus.h>

namespace Debug
{
    static const bool PRINT_METARS = true;
}

namespace Pins
{
    static const uint STATUS_LED = 0;
    static const uint NEOPIXEL = 16;
    static const uint BRIGHTNESS_POT = A1;
    static const uint CONTRAST_POT = A0;
    static const uint LDR = A2;

    static const uint DIP_WIND = 5;
    static const uint DIP_LIGHTNING = 6;
    static const uint DIP_DIMMING = 7;
    static const uint DIP_WIFI_SETUP = 8;
}

namespace Colors
{
    static const uint8_t BRIGHTNESS = 255; // out of 255
    static const uint8_t LIGHTNING_BRIGHTNESS = 255;

    static const RgbColor RED = RgbColor(BRIGHTNESS, 0, 0);
    static const RgbColor GREEN = RgbColor(0, BRIGHTNESS, 0);
    static const RgbColor BLUE = RgbColor(0, 0, BRIGHTNESS);
    static const RgbColor MAGENTA = RgbColor(BRIGHTNESS, 0, BRIGHTNESS);
    static const RgbColor YELLOW = RgbColor(BRIGHTNESS, BRIGHTNESS / 2, 0);
    static const RgbColor BLACK = RgbColor(0);

    static const RgbColor VFR = GREEN;
    static const RgbColor MVFR = BLUE;
    static const RgbColor IFR = RED;
    static const RgbColor LIFR = MAGENTA;

    static const RgbColor LIGHTNING_FLASH = RgbColor(LIGHTNING_BRIGHTNESS);

    static const RgbColor INITIALIZING = GREEN;
    static const RgbColor CONNECTED_NO_DATA = YELLOW;
    static const RgbColor DISCONNECTED = RED;
    static const RgbColor WIFI_SETUP = MAGENTA;
}

namespace Config
{
    // Animation
    static const uint ANIMATION_FRAME_DURATION = 100; // ms
    static const uint GUSTY_WIND_THRESHOLD = 15;      // kt
    static const uint FLICKER_WIND_PERCENT = 20;      // %
    static const uint FLICKER_WIND_DIMMING = 128;     // 255 = original, 0 = black
    static const uint FLASH_LIGHTNING_PERCENT = 5;    // %

    // LEDs
    static const uint MIN_BRIGHTNESS = 5; // out of 255

    // Timing
    static const uint METAR_FETCH_INTERVAL = 300000; // 5 minutes (after successful fetch)
    static const uint METAR_RETRY_INTERVAL = 60000;  // 1 minute (after failed fetch)
    static const uint WIFI_RETRY_INTERVAL = 0;       // immediately (after failed connect)
}

#endif