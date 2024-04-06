#include <Arduino.h>

#include "constants.h"
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

        pinMode(Pins::LDR, INPUT);
        pinMode(Pins::BRIGHTNESS_POT, INPUT);
        pinMode(Pins::CONTRAST_POT, INPUT);
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

        inputs.windVisible = windVisible;
        inputs.lightningVisible = lightningVisible;
        inputs.autoDimming = autoDimming;
        inputs.wifiSetup = wifiSetup;

        float ldr = analogRead(Pins::LDR) / 1024.0;
        float brightness = analogRead(Pins::BRIGHTNESS_POT) / 1024.0;
        float contrast = analogRead(Pins::CONTRAST_POT) / 1024.0;

        // LDR: 5% change threshold
        inputs.ldr = (abs(ldr - _prevInputs.ldr) > 0.05) ? ldr : _prevInputs.ldr;

        // Brightness pot: 5% change threshold
        inputs.brightness = (abs(brightness - _prevInputs.brightness) > 0.05) ? brightness : _prevInputs.brightness;

        // Contrast: 5% change threshold
        inputs.contrast = (abs(contrast - _prevInputs.contrast) > 0.05) ? contrast : _prevInputs.contrast;

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