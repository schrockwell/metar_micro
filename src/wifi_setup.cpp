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
        _server.send(200, "text/html", String(HEADER_HTML) + String(INDEX_HTML) + String(FOOTER_HTML));
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
            _server.send(200, "text/html", String(HEADER_HTML) + String(SUCCESS_HTML) + String(FOOTER_HTML));
        }
        else
        {
            _server.send(400, "text/html", String(HEADER_HTML) + String(ERROR_HTML) + String(FOOTER_HTML));
        }
    }
}