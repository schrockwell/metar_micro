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
        inputs_t inputs;

        // TODO: Debounce
        inputs.windVisible = !digitalRead(Pins::DIP_WIND);
        inputs.lightningVisible = !digitalRead(Pins::DIP_LIGHTNING);
        inputs.autoDimming = !digitalRead(Pins::DIP_DIMMING);
        inputs.wifiSetup = !digitalRead(Pins::DIP_WIFI_SETUP);

        float ldr = analogRead(Pins::LDR) / 1024.0;
        float brightness = analogRead(Pins::BRIGHTNESS_POT) / 1024.0;
        float contrast = analogRead(Pins::CONTRAST_POT) / 1024.0;

        // LDR: 1% change threshold
        inputs.ldr = (abs(ldr - _prevInputs.ldr) > 0.01) ? ldr : _prevInputs.ldr;

        // Brightness pot: 1% change threshold
        inputs.brightness = (abs(brightness - _prevInputs.brightness) > 0.01) ? brightness : _prevInputs.brightness;

        // Contrast: 1% change threshold
        inputs.contrast = (abs(contrast - _prevInputs.contrast) > 0.01) ? contrast : _prevInputs.contrast;

        _prevInputs = inputs;
        return inputs;
    }
}