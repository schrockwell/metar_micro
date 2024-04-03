#include <HTTPClient.h>

#include "faa.h"

namespace FAA
{
  const char *BASE_URL = "https://aviationweather.gov/api/data/metar?format=raw&ids=";

  bool fetchMETARs(const String airportIDs[], const int numberOfAirports, String &metars)
  {
    HTTPClient http;
    http.setInsecure(); // Allow https

    String fullURL = buildURL(BASE_URL, airportIDs, numberOfAirports);
    http.begin(fullURL);
    int httpCode = http.GET();

    if (httpCode > 0)
    {
      metars = http.getString();
      http.end();
      return true;
    }
    else
    {
      http.end();
      return false;
    }
  }

  String buildURL(const String baseURL, const String airportIDs[], const int numberOfAirports)
  {
    String url = baseURL;
    for (int i = 0; i < numberOfAirports; i++)
    {
      if (airportIDs[i] == "")
      {
        continue;
      }

      url += airportIDs[i];
      if (i < numberOfAirports - 1)
      {
        url += ","; // Add a comma after each ID except the last one
      }
    }
    return url;
  }
}