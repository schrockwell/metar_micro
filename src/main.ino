#include <Arduino.h>
#include <NeoPixelBus.h>
#include <WiFi.h>
#include <HTTPClient.h>

#include "airports.h"
#include "constants.h"
#include "secrets.h"

const char *BASE_URL = "https://aviationweather.gov/api/data/metar?format=raw&ids=";

const uint8_t PIXEL_PIN = 16; // make sure to set this to the correct pin, ignored for Esp8266

NeoPixelBus<NeoRgbFeature, NeoWs2812xMethod> _strip(AIRPORT_COUNT, PIXEL_PIN);

status_t _status = NEW;

void setup()
{
  _strip.Begin();
  _strip.Show();
  setStatusLED(INITIALIZING);

  Serial.begin(115200);

  WiFi.begin(SSID, PASSWORD);
}

void loop()
{
  loopMETARFetch();
  loopStatusLED();
}

void loopMETARFetch()
{
  static unsigned long retryAfter = 0;

  if (millis() < retryAfter)
  {
    return;
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;
    http.setInsecure(); // Allow https

    Serial.println("Making request...");

    String fullURL = buildURL(BASE_URL, AIRPORTS, AIRPORT_COUNT);
    http.begin(fullURL);
    int httpCode = http.GET();

    if (httpCode > 0)
    {
      String payload = http.getString();
      displayMETARS(payload);
      Serial.println(payload);

      setStatusLED(CONNECTED_WITH_DATA);
    }
    else
    {
      setStatusLED(CONNECTED_NO_DATA);
      Serial.println("Error on HTTP request");
    }

    http.end();
  }
  else
  {
    setStatusLED(DISCONNECTED);
  }

  retryAfter = millis() + 60000; // Retry every minute
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

void displayMETARS(String lines)
{
  String metar = "";
  for (int i = 0; i < lines.length(); i++)
  {
    if (lines[i] == '\n')
    {
      displayMETAR(metar);
      metar = "";
    }
    else
    {
      metar += lines[i];
    }
  }

  _strip.Show();
}

void displayMETAR(String metar)
{
  // Split METAR into words
  String words[20]; // Assuming a METAR won't have more than 20 words
  int wordCount = 0;
  int startIndex = 0;
  for (int i = 0; i < metar.length(); i++)
  {
    if (metar[i] == ' ')
    {
      words[wordCount++] = metar.substring(startIndex, i);
      startIndex = i + 1;
    }
    else if (i == metar.length() - 1)
    {
      words[wordCount++] = metar.substring(startIndex);
    }

    // Ignore excessive words
    if (wordCount >= 20)
    {
      break;
    }
  }

  // Extract information
  String airportID = words[0];
  int visibility = -1;
  int ceiling = 10000; // Default to high value if not found
  for (int i = 0; i < wordCount; i++)
  {
    // Check if the word contains "SM" for visibility
    if (words[i].endsWith("SM"))
    {
      if (words[i].indexOf("/") == -1)
      {
        // Visibility is in statute miles
        String substr = words[i].substring(0, words[i].length() - 2);
        visibility = substr.toInt();
      }
      else
      {
        // Visibility is in fractions of a mile
        visibility = 0;
      }
    }

    // Check for ceiling information
    if (words[i].startsWith("BKN") || words[i].startsWith("OVC"))
    {
      ceiling = min(ceiling, words[i].substring(3).toInt() * 100);
    }
  }

  category_t flightCategory = getFlightCategory(visibility, ceiling);

  int airportIndex = findAirportIndex(airportID);
  if (airportIndex == -1)
  {
    return;
  }

  switch (flightCategory)
  {
  case VFR:
    _strip.SetPixelColor(airportIndex, VFR_COLOR);
    break;
  case MVFR:
    _strip.SetPixelColor(airportIndex, MVFR_COLOR);
    break;
  case IFR:
    _strip.SetPixelColor(airportIndex, IFR_COLOR);
    break;
  case LIFR:
    _strip.SetPixelColor(airportIndex, LIFR_COLOR);
    break;
  case NA:
    _strip.SetPixelColor(airportIndex, BLACK);
    break;
  }
}

category_t getFlightCategory(int visibility, int ceiling)
{
  if (visibility == -1)
  {
    return NA;
  }
  else if (visibility < 1 || ceiling < 500)
  {
    return LIFR;
  }
  else if (visibility < 3 || ceiling < 1000)
  {
    return IFR;
  }
  else if (visibility < 5 || ceiling < 3000)
  {
    return MVFR;
  }
  else
  {
    return VFR;
  }
}

int findAirportIndex(String airportID)
{
  for (int i = 0; i < AIRPORT_COUNT; i++)
  {
    if (AIRPORTS[i] == airportID)
    {
      return i;
    }
  }
  return -1;
}

void setStatusLED(status_t newStatus)
{
  if (_status == newStatus)
  {
    return;
  }

  switch (newStatus)
  {
  case INITIALIZING:
    clearStrip();
    _strip.SetPixelColor(STATUS_LED, INITIALIZING_COLOR);
    _strip.Show();
    break;
  case CONNECTED_NO_DATA:
    clearStrip();
    _strip.SetPixelColor(STATUS_LED, CONNECTED_NO_DATA_COLOR);
    _strip.Show();
    break;
  case DISCONNECTED:
    clearStrip();
    _strip.SetPixelColor(STATUS_LED, DISCONNECTED_COLOR);
    _strip.Show();
    break;
  }

  _status = newStatus;
}

void loopStatusLED()
{
  // Flash every 500ms
  bool on = millis() % 1000 < 500;

  switch (_status)
  {
  case INITIALIZING:
    _strip.SetPixelColor(STATUS_LED, on ? INITIALIZING_COLOR : BLACK);
    break;
  case CONNECTED_NO_DATA:
    _strip.SetPixelColor(STATUS_LED, on ? CONNECTED_NO_DATA_COLOR : BLACK);
    break;
  case DISCONNECTED:
    _strip.SetPixelColor(STATUS_LED, on ? DISCONNECTED_COLOR : BLACK);
    break;
  }

  _strip.Show();
}

void clearStrip()
{
  for (int i = 0; i < AIRPORT_COUNT; i++)
  {
    _strip.SetPixelColor(i, BLACK);
  }
}