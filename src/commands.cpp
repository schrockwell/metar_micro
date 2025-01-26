#include <Arduino.h>

#include "commands.h"
#include "leds.h"
#include "main.h"
#include "secrets.h"

namespace Commands
{
    char _readBuffer[256];
    int _readIndex = 0;

    void loop()
    {
        if (Serial.available() == 0)
        {
            return;
        }

        char c = Serial.read();

        if (c == '\b') // Handle backspace
        {
            if (_readIndex > 0)
            {
                _readIndex = (_readIndex - 1) % sizeof(_readBuffer);
                _readBuffer[_readIndex] = '\0';
                Serial.print("\b \b"); // erase it in the terminal
            }
            return;
        }
        else if (c != '\n') // Handle regular characters
        {
            Serial.print(c); // echo
            _readBuffer[_readIndex] = c;
            _readIndex = (_readIndex + 1) % sizeof(_readBuffer);
            return;
        }

        // If we made it this far, a newline was received
        Serial.print(c); // echo
        _readBuffer[_readIndex] = '\0';
        _readIndex = 0;

        String command = String(_readBuffer);
        command.trim();

        if (command == "help" || command == "?" || command == "h")
        {
            Serial.println("Commands:");
            Serial.println("  b / brightness [0-100] - Set the brightness (0-100)");
            Serial.println("  f / fetch - Fetch METARs immediately");
            Serial.println("  i / info - Print device info");
            Serial.println("  m / metars - Print METARs");
            Serial.println("  r / reboot - Reboot the device");
            Serial.println("  s / setup - Enter or exit setup mode");
        }
        else if (command == "metars" || command == "m")
        {
            Main::printMetars();
        }
        else if (command == "reboot" || command == "r")
        {
            rp2040.reboot();
        }
        else if (command == "fetch" || command == "f")
        {
            Main::fetchAsap();
        }
        else if (command == "brightness" || command == "b")
        {
            Serial.println("Current brightness: " + String(Main::system.settings.brightness));
        }
        else if (command.startsWith("brightness ") || command.startsWith("b "))
        {
            int startIndex = command.indexOf(" ");
            String brightnessStr = command.substring(startIndex + 1);
            int brightness = brightnessStr.toInt();
            if (brightness < 0 || brightness > 100)
            {
                Serial.println("Brightness must be between 0 and 100");
            }
            else
            {
                Main::system.settings.brightness = brightness;
                Secrets::writeSettings(Main::system.settings);
                LEDs::forceRedraw();
                Serial.println("Brightness set to " + String(Main::system.settings.brightness));
            }
        }
        else if (command == "setup" || command == "s")
        {
            Main::toggleSetup();
        }
        else if (command == "info" || command == "i")
        {
            Serial.println("Model: " + String(Features::MODEL));
            Serial.println("Controller Serial: " + Secrets::getSerialString(Secrets::getControllerSerial()));
            Serial.println("Map Serial: " + Secrets::getSerialString(Secrets::getMapSerial()));
            Serial.println("Firmware: " + String(Version::FIRMWARE));
        }
        else if (command == "")
        {
            // Do nothing
        }
        else
        {
            Serial.println("Unknown command: " + command);
        }

        // Print next command prompt
        Serial.print("airmap> ");
    }
}