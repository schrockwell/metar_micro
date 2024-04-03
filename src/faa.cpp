#include <HTTPClient.h>

#include "faa.h"
#include "metars.h"

namespace FAA
{
  const char *BASE_URL = "https://aviationweather.gov/api/data/metar?format=raw&ids=";

  bool fetchMETARs(metar_t *metars, const int metarCount)
  {
    HTTPClient http;
    http.setInsecure(); // Allow https

    String fullURL = buildURL(BASE_URL, metars, metarCount);
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

  String buildURL(const String baseURL, const metar_t *metars, const int metarCount)
  {
    String url = baseURL;
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
}