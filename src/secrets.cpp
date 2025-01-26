#include "secrets.h"
#include "board.h"

#include <Arduino.h>
#include <EEPROM.h>

namespace Secrets
{
    void setup()
    {
        EEPROM.begin(1024);

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
        // TODO: Migrate old EEPROM settings if known previous version
        Serial.println("Initializing EEPROM");

        settings_t defaultSettings = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASSWORD,
            .lightning = true,
            .wind = true,
            .windy_kts = 15,
            .brightness = Features::DEFAULT_BRIGHTNESS,
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
        EEPROM.put(SIGNATURE_ADDR, SIGNATURE);
        EEPROM.commit();
    }

    bool checkSignature()
    {
        uint32_t readSignature = 0;
        EEPROM.get(SIGNATURE_ADDR, readSignature);
        return readSignature == SIGNATURE;
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