#include "secrets.h"
#include "board.h"

#include <Arduino.h>
#include <EEPROM.h>
#include <OneWire.h>

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
        EEPROM.commit();
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

    uint64_t getControllerSerial()
    {
        if (Pins::CONTROLLER_DS2401 == Pins::NC)
        {
            return getSerialFromPico();
        }
        else
        {
            return getSerialFromDS2401(Pins::CONTROLLER_DS2401);
        }
    }

    uint64_t getMapSerial()
    {
        if (Pins::MAP_DS2401 == Pins::NC)
        {
            return 0;
        }
        else
        {
            return getSerialFromDS2401(Pins::MAP_DS2401);
        }
    }

    String getSerialString(uint64_t serial)
    {
        // convert serial to 0000-0000-0000 format
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

    uint64_t getSerialFromDS2401(uint pin)
    {
        OneWire oneWire(pin);
        uint64_t serial = 0;
        if (oneWire.reset())
        {
            oneWire.write(0x33);
            for (int i = 0; i < 8; i++)
            {
                serial |= (uint64_t)oneWire.read() << (i * 8);
            }
        }
        return serial;
    }
}