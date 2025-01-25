#include "constants.h"
#include "leds.h"
#include "main.h"
#include "secrets.h"
#include "wifi_setup.h"

#include <WiFi.h>
#include <WebServer.h>

namespace WifiSetup
{
    WebServer _server(80);

    void setup()
    {
        _server.on("/", HTTP_GET, handleForm);
        _server.on("/", HTTP_POST, handlePost);
        _server.on("/brightness", HTTP_GET, handleBrightness);
    }

    void loop()
    {
        _server.handleClient();
    }

    void begin()
    {
        Serial.print("Setting soft-AP...");
        Serial.println(WiFi.softAP(SETUP_SSID) ? "Ready" : "Failed");

        Serial.print("Soft-AP IP address: ");
        Serial.println(WiFi.softAPIP());

        _server.begin();

        Serial.println("Web server started");
    }

    void end()
    {
        _server.stop();
        Serial.println("Web server stopped");

        WiFi.softAPdisconnect();
        Serial.println("Soft AP disconnected");

        settings_t settings = Secrets::readSettings();
        WiFi.begin(settings.ssid, settings.password);
    }

    void replaceCheck(String &html, const String &name, bool value)
    {
        html.replace("{{" + name + "}}", value ? "checked" : "");
    }

    void replaceValue(String &html, const String &name, const String &value)
    {
        html.replace("{{" + name + "}}", value);
    }

    void handleForm()
    {
        settings_t settings = Secrets::readSettings();
        String indexHtml = FPSTR(INDEX_HTML);

        replaceValue(indexHtml, "ssid", String(settings.ssid));
        replaceValue(indexHtml, "password", String(settings.password));
        replaceCheck(indexHtml, "lightning", settings.lightning);
        replaceCheck(indexHtml, "wind", settings.wind);
        replaceValue(indexHtml, "windy_kts", String(settings.windy_kts));
        replaceValue(indexHtml, "brightness", String(settings.brightness));

        _server.send(200, "text/html", String(HEADER_HTML) + indexHtml + footerHtml());
    }

    void handlePost()
    {
        String ssid = _server.arg("ssid");
        String password = _server.arg("password");
        String lightning = _server.arg("lightning");
        String wind = _server.arg("wind");
        String windyKts = _server.arg("windy_kts");
        String brightness = _server.arg("brightness");

        Serial.println("SSID: " + ssid);
        Serial.println("Password: " + password);
        Serial.println("Lightning: " + lightning);
        Serial.println("Wind: " + wind);
        Serial.println("Windy kts: " + windyKts);
        Serial.println("Brightness: " + brightness);

        if (ssid.length() > 0 && password.length() > 0)
        {
            settings_t settings = Secrets::readSettings();
            strcpy(settings.ssid, ssid.c_str());
            strcpy(settings.password, password.c_str());
            settings.lightning = lightning == "1";
            settings.wind = wind == "1";
            settings.windy_kts = windyKts.toInt();
            settings.brightness = brightness.toInt();
            Secrets::writeSettings(settings);

            _server.send(200, "text/html", String(HEADER_HTML) + String(SUCCESS_HTML) + footerHtml());
            Main::endSetup();
        }
        else
        {
            _server.send(400, "text/html", String(HEADER_HTML) + String(ERROR_HTML) + footerHtml());
        }
    }

    String footerHtml()
    {
        String html = FPSTR(FOOTER_HTML);

        replaceValue(html, "firmware", Version::FIRMWARE);
        replaceValue(html, "model", Features::MODEL);
        replaceValue(html, "serial", Secrets::getSerialString());

        return html;
    }

    void handleBrightness()
    {
        String brightness = _server.arg("value");
        LEDs::previewBrightness(brightness.toInt());
        _server.send(204, "text/plain", "");
    }
}