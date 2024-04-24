#include "secrets.h"
#include "boards.h"

#include <Arduino.h>
#include <EEPROM.h>

namespace Secrets
{
    void setup()
    {
        EEPROM.begin(512);

        if (!checkSignature())
        {
            writeWiFiCredentials("ssid", "password");
            writeSignature();
        }
    }

    void writeWiFiCredentials(String ssid, String password)
    {
        writeToEEPROM(SSID_ADDR, ssid);
        writeToEEPROM(PASSWORD_ADDR, password);
    }

    void readWiFiCredentials(String &ssid, String &password)
    {
        ssid = readFromEEPROM(SSID_ADDR);
        password = readFromEEPROM(PASSWORD_ADDR);
    }

    void writeSignature()
    {
        for (int i = 0; i < sizeof(SIGNATURE); ++i)
        {
            EEPROM.write(SIGNATURE_ADDR + i, (SIGNATURE >> (8 * i)) & 0xFF);
        }
        EEPROM.commit();
    }

    bool checkSignature()
    {
        uint32_t readSignature = 0;
        for (int i = 0; i < sizeof(SIGNATURE); ++i)
        {
            readSignature |= (uint32_t)EEPROM.read(SIGNATURE_ADDR + i) << (8 * i);
        }
        return readSignature == SIGNATURE;
    }

    void writeToEEPROM(int startAddr, const String &data)
    {
        int i;
        for (i = 0; i < data.length(); ++i)
        {
            EEPROM.write(startAddr + i, data[i]);
        }
        EEPROM.write(startAddr + i, '\0'); // Add null terminator to mark end of string
        EEPROM.commit();                   // Make sure to commit changes to EEPROM
    }

    String readFromEEPROM(int startAddr)
    {
        String result;
        char ch;
        while (true)
        {
            ch = EEPROM.read(startAddr++);
            if (ch == '\0')
            {
                break;
            }
            result += ch;
        }
        return result;
    }

    uint64_t getSerial()
    {
        if (Features::HARDCODED_SERIAL != 0)
        {
            return Features::HARDCODED_SERIAL;
        }

        // TODO: implement serial number reader from IC

        return 0;
    }
}