#include <Arduino.h>

#include "constants.h"
#include "board.h"
#include "inputs.h"

namespace Inputs
{
    inputs_t _prevInputs;

    void setup()
    {
        pinMode(Pins::DIP_WIND, INPUT_PULLUP);
        pinMode(Pins::DIP_LIGHTNING, INPUT_PULLUP);
        pinMode(Pins::DIP_DIMMING, INPUT_PULLUP);
        pinMode(Pins::DIP_WIFI_SETUP, INPUT_PULLUP);

        if (Features::MAX_BRIGHTNESS_POT)
        {
            pinMode(Pins::MAX_BRIGHTNESS_POT, INPUT);
        }

        if (Features::MIN_BRIGHTNESS_POT)
        {
            pinMode(Pins::MIN_BRIGHTNESS_POT, INPUT);
        }

        if (Features::LDR)
        {
            pinMode(Pins::LDR, INPUT);
        }
    }

    inputs_t read()
    {
        static bool wifiSetup;
        static unsigned long wifiSetupPrevMillis = 0;

        inputs_t inputs;

        debounce(LONG_PRESS_DELAY, !digitalRead(Pins::DIP_WIFI_SETUP), wifiSetup, wifiSetupPrevMillis);

        inputs.wifiSetup = wifiSetup || Features::FORCE_WIFI_SETUP;

        float ldr = Features::LDR ? analogRead(Pins::LDR) / 1024.0 : Features::DEFAULT_LDR;

        // LDR: 5% change threshold
        inputs.ldr = (abs(ldr - _prevInputs.ldr) > 0.05) ? ldr : _prevInputs.ldr;

        _prevInputs = inputs;
        return inputs;
    }

    void debounce(int delay, bool value, bool &prevValue, unsigned long &prevMillis)
    {
        if (value != prevValue)
        {
            if (millis() - prevMillis > delay)
            {
                prevValue = value;
            }
            prevMillis = millis();
        }
    }
}