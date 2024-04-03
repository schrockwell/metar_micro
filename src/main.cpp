#include <Arduino.h>
#include <NeoPixelBus.h>
#include <WiFi.h>
#include <HTTPClient.h>

#include "airports.h"
#include "constants.h"
#include "main.h"
#include "metars.h"
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
  String airportID = parseAirportID(metar);
  int airportIndex = findAirportIndex(airportID);
  if (airportIndex == -1)
  {
    return;
  }

  category_t flightCategory = parseFlightCategory(metar);
  switch (flightCategory)
  {
  case VFR:
    _strip.SetPixelColor(airportIndex, Colors::VFR);
    break;
  case MVFR:
    _strip.SetPixelColor(airportIndex, Colors::MVFR);
    break;
  case IFR:
    _strip.SetPixelColor(airportIndex, Colors::IFR);
    break;
  case LIFR:
    _strip.SetPixelColor(airportIndex, Colors::LIFR);
    break;
  case NA:
    _strip.SetPixelColor(airportIndex, Colors::BLACK);
    break;
  }
}

int findAirportIndex(String airportID)
{
  if (airportID == "")
  {
    return -1;
  }

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
    _strip.SetPixelColor(STATUS_LED, Colors::INITIALIZING);
    _strip.Show();
    break;
  case CONNECTED_NO_DATA:
    clearStrip();
    _strip.SetPixelColor(STATUS_LED, Colors::CONNECTED_NO_DATA);
    _strip.Show();
    break;
  case DISCONNECTED:
    clearStrip();
    _strip.SetPixelColor(STATUS_LED, Colors::DISCONNECTED);
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
    _strip.SetPixelColor(STATUS_LED, on ? Colors::INITIALIZING : Colors::BLACK);
    break;
  case CONNECTED_NO_DATA:
    _strip.SetPixelColor(STATUS_LED, on ? Colors::CONNECTED_NO_DATA : Colors::BLACK);
    break;
  case DISCONNECTED:
    _strip.SetPixelColor(STATUS_LED, on ? Colors::DISCONNECTED : Colors::BLACK);
    break;
  }

  _strip.Show();
}

void clearStrip()
{
  for (int i = 0; i < AIRPORT_COUNT; i++)
  {
    _strip.SetPixelColor(i, Colors::BLACK);
  }
}