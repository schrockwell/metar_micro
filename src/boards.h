#ifndef BOARDS_H
#define BOARDS_H

#include <Arduino.h>

#ifdef AIRMAP_BOARD_PROTO

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

#endif // AIRMAP_BOARD_PROTO

#ifdef AIRMAP_BOARD_DEV1

#define NEOPIXEL_ORDERING NeoGrbFeature

namespace Pins
{
    static const uint STATUS_LED = 0;
    static const uint NEOPIXEL = 16;
    static const uint MAX_BRIGHTNESS_POT = A1;
    static const uint MIN_BRIGHTNESS_POT = A0;
    static const uint LDR = A2;

    static const uint DIP_WIND = 8;
    static const uint DIP_LIGHTNING = 7;
    static const uint DIP_DIMMING = 6;
    static const uint DIP_WIFI_SETUP = 5;
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

    // LED current draw (~12 mA per LED at full brightness):
    //   255 -> 710 mA
    //   128 -> 360 mA
    //   64  -> 160 mA
    //   32  -> 60 mA
    // (+100 mA for Pico)
    static const uint8_t MASTER_MIN_BRIGHTNESS = 2;  // 0-255
    static const uint8_t MASTER_MAX_BRIGHTNESS = 64; // 0-255

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
        "KFZY",
        "KSYR",
        "KVGC",
        "KOIC",
        "NY0",
        "KSCH",
        "5B2",
        "KALB",
        "KAQW",
        "KEEN",
        "KORE",
        "KFIT",
        "KASH",
        "KMHT",
        "KLWM",
        "KBVY",
        "KPYM",
        "KBOS",
        "KOWD",
        "KBED",
        "KSFZ",
        "KORH",
        "KCEF",
        "KBDL",
        "KBAF",
        "KPSF",
        "1B1",
        "N66",
        "N23",
        "KBGM",
        "KMSV",
        "KMGJ",
        "KSWF",
        "KPOU",
        "KDXR",
        "KOXC",
        "KHFD",
        "KIJD",
        "KWST",
        "KPVD",
        "KOQU",
        "KEWB",
        "KMVY",
        "KHYA",
        "KACK",
        "KGON",
        "KFOK",
        "KHWV",
        "KHVN",
        "KISP",
        "KBDR",
        "KTEB",
        "KCDW",
        "KMMU",
        "KMPO",
        "KAVP",
        "KHZL",
        "KZER",
        "KRDG",
        "KABE",
        "KCKZ",
        "N51",
        "KLDJ",
        "KEWR",
        "KLGA",
        "KJFK",
        "KFRG",
    };

    static const int COUNT = sizeof(IDs) / sizeof(IDs[0]);
}

#endif // AIRMAP_BOARD_DEV1

#endif // BOARDS_H