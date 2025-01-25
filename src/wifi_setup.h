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

    static const char HEADER_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>Wi-Fi Setup</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: Arial; background-color: #e2e8f0; }
        fieldset { border: 0; padding: 0; margin: 2em 0; max-width: 20em; }
        fieldset.h { display: flex; align-items: center; gap: 0.5em; }
        fieldset.v { display: flex; flex-direction: column; gap: 0.5em; }
        input { max-width: 20em; }
        input[type=text], input[type=password] { padding: 0.5em 1em; font-size: 1.25em; }
        input[type=submit] { padding: 0.5em 1em; font-size: 1.25em; background-color: #2563eb; color: white; border: 0; cursor: pointer; }
        a { color: #2563eb; }
    </style>
</head>
<body>
    )rawliteral";

    static const char FOOTER_HTML[] PROGMEM = R"rawliteral(
</body>
</html>
)rawliteral";

    static const char INDEX_HTML[] PROGMEM = R"rawliteral(
<h2>METAR Map Wi-Fi Setup</h2>
<form method="post">
    <fieldset class="v">
        <label for="ssid">SSID</label>
        <input type="text" id="ssid" name="ssid" value="{{ssid}}">
    </fieldset>

    <fieldset class="v">
        <label for="password">Password</label>
        <input type="text" id="password" name="password" value="{{password}}">
    </fieldset>

    <fieldset class="h">
        <input type="checkbox" id="lightning" name="lightning" value="1" {{lightning}}>
        <label for="lightning">Show lightning</label>
    </fieldset>

    <fieldset class="v">
        <label for="windy_kts">Show windy stations above kts</label>
        <input type="text" id="windy_kts" name="windy_kts" value="{{windy_kts}}">
    </fieldset>

    <fieldset class="v">
        <label for="brightness">Brightness</label>
        <input type="range" min="5" max="100" id="brightness" name="brightness" value="{{brightness}}">
    </fieldset>
    
    <input type="submit" value="Save">
</form>
)rawliteral";

    static const char SUCCESS_HTML[] PROGMEM = R"rawliteral(
<p>Wi-Fi credentials saved. You can switch back to normal mode now.</p>
)rawliteral";

    static const char ERROR_HTML[] PROGMEM = R"rawliteral(
<p>Failed to save Wi-Fi credentials. <a href="/">Please try again.</a></p>
)rawliteral";

}

#endif // WIFI_SETUP_H