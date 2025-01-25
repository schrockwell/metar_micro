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
        static bool windVisible;
        static bool lightningVisible;
        static bool autoDimming;
        static bool wifiSetup;
        static unsigned long windVisibilePrevMillis = 0;
        static unsigned long lightningVisiblePrevMillis = 0;
        static unsigned long autoDimmingPrevMillis = 0;
        static unsigned long wifiSetupPrevMillis = 0;

        inputs_t inputs;

        debounce(!digitalRead(Pins::DIP_WIND), windVisible, windVisibilePrevMillis);
        debounce(!digitalRead(Pins::DIP_LIGHTNING), lightningVisible, lightningVisiblePrevMillis);
        debounce(!digitalRead(Pins::DIP_DIMMING), autoDimming, autoDimmingPrevMillis);
        debounce(!digitalRead(Pins::DIP_WIFI_SETUP), wifiSetup, wifiSetupPrevMillis);

        inputs.wifiSetup = wifiSetup || Features::FORCE_WIFI_SETUP;

        float ldr = Features::LDR ? analogRead(Pins::LDR) / 1024.0 : Features::DEFAULT_LDR;
        float maxBrightness = Features::MAX_BRIGHTNESS_POT ? analogRead(Pins::MAX_BRIGHTNESS_POT) / 1024.0 : Features::DEFAULT_MAX_BRIGHTNESS;
        float minBrightness = Features::MIN_BRIGHTNESS_POT ? analogRead(Pins::MIN_BRIGHTNESS_POT) / 1024.0 : Features::DEFAULT_MIN_BRIGHTNESS;

        // LDR: 5% change threshold
        inputs.ldr = (abs(ldr - _prevInputs.ldr) > 0.05) ? ldr : _prevInputs.ldr;

        _prevInputs = inputs;
        return inputs;
    }

    void debounce(bool value, bool &prevValue, unsigned long &prevMillis)
    {
        if (value != prevValue)
        {
            if (millis() - prevMillis > DEBOUNCE_DELAY)
            {
                prevValue = value;
                prevMillis = millis();
            }
        }
    }
}