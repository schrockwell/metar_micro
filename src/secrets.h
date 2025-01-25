#ifndef SECRETS_H
#define SECRETS_H

#include <Arduino.h>

#ifndef WIFI_SSID
#define WIFI_SSID ""
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD ""
#endif

#include "types.h"

namespace Secrets
{
    static const int SIGNATURE_ADDR = 0; // Start address for magic number
    static const int SETTINGS_ADDR = 4;  // Start address for password

    static const uint32_t SIGNATURE = 0xFABCAB00;

    void setup();

    void initializeEEPROM();

    void writeSettings(settings_t settings);
    settings_t readSettings();

    void writeSignature();
    bool checkSignature();

    uint64_t getSerial();
    uint64_t getSerialFromPico();
    String getSerialString();
}

#endif // SECRETS_H