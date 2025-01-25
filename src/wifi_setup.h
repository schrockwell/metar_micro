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
    void handleBrightness();

    static const char HEADER_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>Airmap Setup</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: Arial; background-color: #e2e8f0; }
        form { width: 20em; }
        fieldset { border: 0; padding: 0; margin: 2em 0; max-width: 20em; }
        input[type=text], input[type=password], input[type=number] { padding: 0.5em; font-size: 1.25em; width: 100% }
        input[type=submit] { padding: 0.5em 1em; font-size: 1.25em; background-color: #2563eb; color: white; border: 0; cursor: pointer; border-radius: 0.25em; }
        a { color: #2563eb; }
        hr { border: 0; border-top: 1px solid #d2d6dc; margin: 2em 0; width: 100%; }
        p.notice { font-size: 1.5em; text-align: center; padding: 1em; }
        .flex-h { display: flex; align-items: center; gap: 0.5em; }
        .flex-v { display: flex; flex-direction: column; gap: 0.5em; }
    </style>
    <script>
        document.addEventListener('DOMContentLoaded', function() {
            document.getElementById('brightness').addEventListener('input', function(e) {
                fetch('/brightness?value=' + e.target.value);
            });
        })
    </script>
</head>
<body>
    )rawliteral";

    static const char FOOTER_HTML[] PROGMEM = R"rawliteral(
</body>
</html>
)rawliteral";

    static const char INDEX_HTML[] PROGMEM = R"rawliteral(
<h2>Airmap Setup</h2>
<form method="post">
    <fieldset class="flex-v">
        <label for="ssid">Wi-Fi SSID</label>
        <input type="text" autocorrect="off" autocapitalize="off" autocomplete="off" spellcheck="false" id="ssid" name="ssid" value="{{ssid}}" required>
    </fieldset>

    <fieldset class="flex-v">
        <label for="password">Wi-Fi Password</label>
        <input type="text" autocorrect="off" autocapitalize="off" autocomplete="off" spellcheck="false" id="password" name="password" value="{{password}}" required>
    </fieldset>

    <hr>

    <fieldset class="flex-v">
        <div class="flex-h">
            <input type="checkbox" id="wind" name="wind" value="1" {{wind}}>
            <label for="wind">Flicker stations gusting above</label>
        </div>
        <div class="flex-h">
            <input type="number" min="0" step="1" id="windy_kts" name="windy_kts" value="{{windy_kts}}" style="width: 3em;" pattern="\d*" inputmode="numeric">
            <span>kts</span>
        </div>
    </fieldset>

    <fieldset class="flex-h">
        <input type="checkbox" id="lightning" name="lightning" value="1" {{lightning}}>
        <label for="lightning">Flash stations with lightning</label>
    </fieldset>

    <fieldset class="flex-v">
        <label for="brightness">Brightness</label>
        <input type="range" min="5" max="100" step="5" id="brightness" name="brightness" value="{{brightness}}">
    </fieldset>
    
    <input type="submit" value="Save">
</form>
)rawliteral";

    static const char SUCCESS_HTML[] PROGMEM = R"rawliteral(
<p class="notice">Settings saved. The map will now connect to the internet. To make further changes, long-press the setup button.</p>
)rawliteral";

    static const char ERROR_HTML[] PROGMEM = R"rawliteral(
<p class="notice">Failed to save settings. <a href="/">Please try again.</a></p>
)rawliteral";

}

#endif // WIFI_SETUP_H