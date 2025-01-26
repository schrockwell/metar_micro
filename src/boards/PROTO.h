#include <Arduino.h>

#define NEOPIXEL_ORDERING NeoRgbFeature

namespace Pins
{
    static const uint STATUS_LED = 0;
    static const uint NEOPIXEL = 16;
    // static const uint MAX_BRIGHTNESS_POT = A1; // Not used
    // static const uint MIN_BRIGHTNESS_POT = A0; // Not used
    static const uint LDR = A2;

    static const uint DIP_WIND = 5;
    static const uint DIP_LIGHTNING = 6;
    static const uint DIP_DIMMING = 7;
    static const uint DIP_WIFI_SETUP = 8;

    static const uint CONTROLLER_DS2401 = NC;
    static const uint MAP_DS2401 = NC;
}

namespace Features
{
    static const String MODEL = "PROTO";

    // Analog inputs
    static const bool LDR = true;

    // Defaults
    static const float DEFAULT_LDR = 1.0;         // 0-1
    static const uint8_t DEFAULT_BRIGHTNESS = 50; // 0-100

    // LEDs
    static const uint8_t MASTER_MIN_BRIGHTNESS = 10;  // 0-255
    static const uint8_t MASTER_MAX_BRIGHTNESS = 255; // 0-255
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