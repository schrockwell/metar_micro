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

        String ssid;
        String password;
        Secrets::readWiFiCredentials(ssid, password);
        WiFi.begin(ssid.c_str(), password.c_str());
    }

    void handleForm()
    {
        const String html = F("<!DOCTYPE html><html><body><h1>WiFi Setup</h1><form method=\"post\">SSID:<br><input type=\"text\" name=\"ssid\"><br>Password:<br><input type=\"password\" name=\"password\"><br><br><input type=\"submit\" value=\"Submit\"></form></body></html>");
        _server.send(200, "text/html", html);
    }

    void handlePost()
    {
        String ssid = _server.arg("ssid");
        String password = _server.arg("password");

        Serial.println("SSID: " + ssid);
        Serial.println("Password: " + password);

        if (ssid.length() > 0 && password.length() > 0)
        {
            Secrets::writeWiFiCredentials(ssid, password);
            _server.send(200, "text/plain", "Saved. You can switch back to normal mode now.");
        }
        else
        {
            _server.send(400, "text/plain", "Invalid");
        }
    }
}