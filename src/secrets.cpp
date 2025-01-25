#include "secrets.h"
#include "board.h"

#include <Arduino.h>
#include <EEPROM.h>

namespace Secrets
{
    void setup()
    {
        EEPROM.begin(512);

        if (checkSignature())
        {
            Serial.println("EEPROM signature found");
        }
        else
        {
            Serial.println("No EEPROM signature found");
            initializeEEPROM();
        }
    }

    void initializeEEPROM()
    {
        Serial.println("Initializing EEPROM");

        settings_t defaultSettings = {
            .ssid = "",
            .password = "",
            .lightning = true,
            .windy_kts = 15,
            .brightness = 50,
        };

        writeSettings(defaultSettings);
        writeSignature();
    }

    void writeSettings(settings_t settings)
    {
        EEPROM.put(SETTINGS_ADDR, settings);
    }

    settings_t readSettings()
    {
        settings_t settings;
        EEPROM.get(SETTINGS_ADDR, settings);
        return settings;
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
        return getSerialFromPico();
    }

    String getSerialString()
    {
        // convert serial to 0000-0000-0000 format
        uint64_t serial = getSerial();
        char serialStr[16];
        snprintf(serialStr, sizeof(serialStr), "%04X-%04X-%04X", (uint16_t)(serial >> 32), (uint16_t)(serial >> 16), (uint16_t)serial);
        return String(serialStr);
    }

    uint64_t getSerialFromPico()
    {
        pico_unique_board_id_t id;
        pico_get_unique_board_id(&id);
        return *(uint64_t *)id.id & 0x0000FFFFFFFFFFFF;
    }
}