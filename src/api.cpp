#include <HTTPClient.h>

#include "api.h"
#include "metars.h"
#include "secrets.h"

namespace API
{
    const char *BASE_URL = "http://192.168.1.227:4000";

    bool fetchMETARs(metar_t *metars, const int metarCount)
    {
        HTTPClient http;
        http.setInsecure(); // Allow https

        String fullURL = buildURL(BASE_URL, Secrets::getSerial());
        Serial.println(fullURL);
        http.begin(fullURL);
        int httpCode = http.GET();
        bool ok = false;

        if (httpCode == 200)
        {
            ok = true;

            String payload;
            payload = http.getString();

            METARS::parseMETARs(payload, metars, metarCount);

            Serial.print(payload);
            Serial.println("-----------------------------------");
        }
        else if (httpCode == 0)
        {
            Serial.println("HTTP GET failed");
        }
        else
        {
            Serial.println("HTTP GET error response: " + http.errorToString(httpCode));
        }

        http.end();
        return ok;
    }

    String buildURL(const String baseURL, const uint64_t serial)
    {
        // Convert serial to 12 digit hex string
        char serialStr[13];
        snprintf(serialStr, sizeof(serialStr), "%012llx", serial);

        return baseURL + "/api/devices/" + serialStr + "/metars";
    }
}