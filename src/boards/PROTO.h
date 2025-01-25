#include <Arduino.h>

#define NEOPIXEL_ORDERING NeoRgbFeature

namespace Pins
{
    static const uint STATUS_LED = 0;
    static const uint NEOPIXEL = 16;
    static const uint MAX_BRIGHTNESS_POT = A1;
    static const uint MIN_BRIGHTNESS_POT = A0;
    static const uint LDR = A2;

    static const uint DIP_WIND = 5;
    static const uint DIP_LIGHTNING = 6;
    static const uint DIP_DIMMING = 7;
    static const uint DIP_WIFI_SETUP = 8;
}

namespace Features
{
    // Analog inputs
    static const bool MAX_BRIGHTNESS_POT = true;
    static const bool MIN_BRIGHTNESS_POT = true;
    static const bool LDR = true;

    static const float DEFAULT_MAX_BRIGHTNESS = 1.0;
    static const float DEFAULT_MIN_BRIGHTNESS = 1.0;
    static const float DEFAULT_LDR = 1.0;

    // LEDs
    static const uint8_t MASTER_MIN_BRIGHTNESS = 10;  // 0-255
    static const uint8_t MASTER_MAX_BRIGHTNESS = 255; // 0-255

    // Switches
    static const bool FORCE_WINDS = false;
    static const bool FORCE_LIGHTNING = false;
    static const bool FORCE_DIMMING = false;
    static const bool FORCE_WIFI_SETUP = false;
}

namespace Airports
{
    static const String IDs[] = {
        "KSFM",
        "KDAW",
        "KCON",
        "KVSF",
        "KRUT",
        "KGFL",
        "KRME",
    };

    static const int COUNT = sizeof(IDs) / sizeof(IDs[0]);
}