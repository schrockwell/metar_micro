#ifndef SECRETS_H
#define SECRETS_H

#include <Arduino.h>

namespace Secrets
{
    static const int SIGNATURE_ADDR = 0;  // Start address for magic number
    static const int SSID_ADDR = 4;       // Start address for SSID
    static const int PASSWORD_ADDR = 100; // Start address for password

    static const uint32_t SIGNATURE = 0xDEADBEEF;

    void setup();

    void writeWiFiCredentials(String ssid, String password);
    void readWiFiCredentials(String &ssid, String &password);

    void writeSignature();
    bool checkSignature();

    void writeToEEPROM(int startAddr, const String &data);
    String readFromEEPROM(int startAddr);

    uint64_t getSerial();
    uint64_t getSerialFromPico();
    String getSerialString();

}

#endif // SECRETS_H