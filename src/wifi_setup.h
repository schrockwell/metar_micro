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
    <style>
        body { font-family: Arial; background-color: #e2e8f0; }
        fieldset { border: 0; padding: 0; margin: 1em 0; }
        label { display: block; margin-bottom: 0.5em; }
        input[type=text], input[type=password] { padding: 0.5em; }
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
    <fieldset>
        <label for="ssid">SSID</label>
        <input type="text" name="ssid">
    </fieldset>

    <fieldset>
        <label for="password">Password</label>
        <input type="password" name="password">
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