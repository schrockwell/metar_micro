#ifndef WIFI_SETUP_H
#define WIFI_SETUP_H

#include <WiFi.h>

namespace WifiSetup
{
    static const char *SETUP_SSID = "METAR Map Setup";

    void setup();
    void loop();

    void begin();
    void end();

    void handleForm();
    void handlePost();
}

#endif // WIFI_SETUP_H