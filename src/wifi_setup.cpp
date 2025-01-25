#include "wifi_setup.h"
#include "secrets.h"

#include <WiFi.h>
#include <WebServer.h>

namespace WifiSetup
{
    WebServer _server(80);

    void setup()
    {
        _server.on("/", HTTP_GET, handleForm);
        _server.on("/", HTTP_POST, handlePost);
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

        Secrets::settings_t settings = Secrets::readSettings();
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
        Secrets::settings_t settings = Secrets::readSettings();
        String indexHtml = FPSTR(INDEX_HTML);

        replaceValue(indexHtml, "ssid", String(settings.ssid));
        replaceValue(indexHtml, "password", String(settings.password));
        replaceCheck(indexHtml, "lightning", settings.lightning);
        replaceValue(indexHtml, "windy_kts", String(settings.windy_kts));
        replaceValue(indexHtml, "brightness", String(settings.brightness));

        _server.send(200, "text/html", String(HEADER_HTML) + indexHtml + String(FOOTER_HTML));
    }

    void handlePost()
    {
        String ssid = _server.arg("ssid");
        String password = _server.arg("password");
        String lightning = _server.arg("lightning");
        String windyKts = _server.arg("windy_kts");
        String brightness = _server.arg("brightness");

        Serial.println("SSID: " + ssid);
        Serial.println("Password: " + password);
        Serial.println("Lightning: " + lightning);
        Serial.println("Windy kts: " + windyKts);
        Serial.println("Brightness: " + brightness);

        if (ssid.length() > 0 && password.length() > 0)
        {
            Secrets::settings_t settings = Secrets::readSettings();
            strcpy(settings.ssid, ssid.c_str());
            strcpy(settings.password, password.c_str());
            settings.lightning = lightning == "1";
            settings.windy_kts = windyKts.toInt();
            settings.brightness = brightness.toInt();
            Secrets::writeSettings(settings);

            _server.send(200, "text/html", String(HEADER_HTML) + String(SUCCESS_HTML) + String(FOOTER_HTML));
        }
        else
        {
            _server.send(400, "text/html", String(HEADER_HTML) + String(ERROR_HTML) + String(FOOTER_HTML));
        }
    }
}