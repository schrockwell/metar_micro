#include <Arduino.h>

#define NEOPIXEL_ORDERING NeoGrbFeature

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
    static const String MODEL = "AM-M00810NE-1";

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
    // TODO: FIX THIS ORDERING! IT IS WRONG!
    static const String IDs[] = {
        "KGAI",
        "KALB",
        "KMVM",
        "KASH",
        "KSFZ",
        "KJST",
        "KOQU",
        "KFOK",
        "KPSF",
        "KCGS",
        "KMMK",
        "CYVO",
        "KBGR",
        "KAOO",
        "KRVL",
        "KCGE",
        "KESN",
        "CYOW",
        "KVSF",
        "KHMZ",
        "KMMU",
        "KEWB",
        "KORE",
        "KLKP",
        "KLRG",
        "KLCI",
        "KMGW",
        "KAFJ",
        "KEWR",
        "KSLK",
        "KHVN",
        "KPJC",
        "KANP",
        "KADW",
        "KFMH",
        "KHGR",
        "KAGC",
        "KFWN",
        "KLEB",
        "KVAY",
        "KJYO",
        "KRME",
        "KPOU",
        "KBVY",
        "KITH",
        "KGBR",
        "KGED",
        "KCAR",
        "KBOS",
        "KFRR",
        "KBED",
        "KHWV",
        "KMVY",
        "KMHT",
        "KAPG",
        "KCKZ",
        "KWAY",
        "KPNE",
        "KEEN",
        "KPBG",
        "KJHW",
        "KHIE",
        "KPEO",
        "KLGA",
        "KLHV",
        "KFVE",
        "KSEG",
        "KTHV",
        "KPTD",
        "KWST",
        "CYMX",
        "KOXC",
        "KPSM",
        "KEFK",
        "KMQS",
        "KDCA",
        "KOXB",
        "KMVL",
        "KBUF",
        "KDYL",
        "CYND",
        "KTEB",
        "KLWM",
        "KPHL",
        "KCDA",
        "KACY",
        "KSYR",
        "KCBE",
        "KOWD",
        "KOIC",
        "KGHG",
        "KNEL",
        "KMPO",
        "KDSV",
        "KFRG",
        "KHYA",
        "KMIV",
        "KOWK",
        "KGFL",
        "KELM",
        "KWVL",
        "KPTW",
        "KSDC",
        "CYSN",
        "KDAA",
        "KRUT",
        "KERR",
        "KPNN",
        "KMRB",
        "KFIG",
        "KFIT",
        "KLBE",
        "KWRI",
        "CYHU",
        "KIAG",
        "KCQX",
        "KHFD",
        "KBDL",
        "CYPQ",
        "CYUL",
        "KGVQ",
        "KFWQ",
        "CYGK",
        "KDKK",
        "KBTV",
        "KMLT",
        "CYOO",
        "KSWF",
        "KGTB",
        "KZER",
        "KART",
        "CYSL",
        "KRDG",
        "KIPT",
        "KBDR",
        "KIDI",
        "KIAD",
        "KAUG",
        "KOLE",
        "KBFD",
        "KUNV",
        "KIWI",
        "KABE",
        "KPIT",
        "KBST",
        "CYSC",
        "KORH",
        "KCON",
        "KBWI",
        "KPVD",
        "KEVY",
        "KMSS",
        "KMAL",
        "KMPV",
        "KDAW",
        "KGON",
        "KBAF",
        "KMSV",
        "KMTN",
        "KCKB",
        "KPVC",
        "KPYM",
        "KAXQ",
        "KVGC",
        "KOLD",
        "KPQI",
        "KDXR",
        "KTTN",
        "KWWD",
        "KROC",
        "KBTP",
        "KSFM",
        "KJFK",
        "KLEW",
        "CYRQ",
        "KPWM",
        "KOQN",
        "KOGS",
        "KHUL",
        "KIZG",
        "KBML",
        "KFZY",
        "KIUA",
        "KVVS",
        "KSNC",
        "KAVP",
        "KLOM",
        "KRKD",
        "KCXY",
        "KBID",
        "KSMQ",
        "KBGM",
        "KJPX",
        "KDOV",
        "KFSO",
        "KDUJ",
        "KBLM",
        "KOKV",
        "KELZ",
        "KACK",
        "KLNS",
        "KTAN",
        "KAQW",
        "CYQB",
        "CYTR",
        "KILG",
        "KFDK",
        "KOYM",
        "KMTP",
        "KFKL",
        "KDMW",
        "KGDM",
        "KISP",
        "KHPN",
        "KDDH",
        "KHZL",
        "KBHB",
        "KPSB",
        "KAFN",
        "KIJD",
        "KMUI",
    };

    static const int COUNT = sizeof(IDs) / sizeof(IDs[0]);
}