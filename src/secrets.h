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

    uint64_t getControllerSerial();
    uint64_t getMapSerial();
    uint64_t getSerialFromPico();
    uint64_t getSerialFromDS2401(uint pin);
    String getSerialString(uint64_t serial);
}

#endif // SECRETS_H