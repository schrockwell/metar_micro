#include <HTTPClient.h>

#include "faa.h"
#include "metars.h"
#include "secrets.h"

namespace FAA
{
  const char *FAA_BASE_URL = "https://aviationweather.gov/api/data/metar?format=raw&ids=";
  const char *API_BASE_URL = "https://tropo.airmap.shop";

  bool fetchMETARs(data_source_t source, metar_t *metars, const int metarCount)
  {
    HTTPClient http;
    String fullURL;

    switch (source)
    {
    case data_source_t::FAA_SOURCE:
      http.setInsecure(); // Allow https
      fullURL = buildFAAURL(metars, metarCount);
      break;
    case data_source_t::API_SOURCE:
      http.setInsecure(); // Allow https
      fullURL = buildAPIURL();
      break;
    }

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

  String buildFAAURL(const metar_t *metars, const int metarCount)
  {
    String url = FAA_BASE_URL;
    for (int i = 0; i < metarCount; i++)
    {
      url += metars[i].airportID;
      if (i < metarCount - 1)
      {
        url += ","; // Add a comma after each ID except the last one
      }
    }
    return url;
  }

  String buildAPIURL()
  {
    String url = API_BASE_URL;
    // Convert serial to 12 digit hex string
    char serialStr[13];
    snprintf(serialStr, sizeof(serialStr), "%012llx", Secrets::getSerial());

    url += "/api/devices/" + String(serialStr) + "/metars";

    return url;
  }
}