#include <Arduino.h>
#include <NeoPixelBus.h>
#include <WiFi.h>

#include "airports.h"
#include "constants.h"
#include "faa.h"
#include "main.h"
#include "metars.h"
#include "secrets.h"

void setup()
{
  Main::setupStrip();

  Serial.begin(115200);

  WiFi.begin(Secrets::SSID, Secrets::PASSWORD);
}

void loop()
{
  Main::loopMETARFetch();
  Main::loopStatusLED();
}

namespace Main
{
  NeoPixelBus<NeoRgbFeature, NeoWs2812xMethod> _strip(Airports::COUNT, Pins::NEOPIXEL);

  status_t _status = NEW;

  void setupStrip()
  {
    _strip.Begin();
    _strip.Show();

    setStatusLED(INITIALIZING);
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
      String payload;

      if (FAA::fetchMETARs(Airports::IDs, Airports::COUNT, payload))
      {
        displayMETARs(payload);
        setStatusLED(CONNECTED_WITH_DATA);
      }
      else
      {
        setStatusLED(CONNECTED_NO_DATA);
      }
    }
    else
    {
      setStatusLED(DISCONNECTED);
    }

    retryAfter = millis() + 60000; // Retry every minute
  }

  void displayMETARs(String payload)
  {
    String metar = "";
    for (int i = 0; i < payload.length(); i++)
    {
      if (payload[i] == '\n')
      {
        displayMETAR(metar);
        metar = "";
      }
      else
      {
        metar += payload[i];
      }
    }

    _strip.Show();
  }

  void displayMETAR(String metar)
  {
    String airportID = METARS::parseAirportID(metar);
    int airportIndex = Airports::findAirportIndex(airportID);
    if (airportIndex == -1)
    {
      return;
    }

    category_t flightCategory = METARS::parseFlightCategory(metar);
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
      _strip.SetPixelColor(Pins::STATUS_LED, Colors::INITIALIZING);
      _strip.Show();
      break;
    case CONNECTED_NO_DATA:
      clearStrip();
      _strip.SetPixelColor(Pins::STATUS_LED, Colors::CONNECTED_NO_DATA);
      _strip.Show();
      break;
    case DISCONNECTED:
      clearStrip();
      _strip.SetPixelColor(Pins::STATUS_LED, Colors::DISCONNECTED);
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
      _strip.SetPixelColor(Pins::STATUS_LED, on ? Colors::INITIALIZING : Colors::BLACK);
      break;
    case CONNECTED_NO_DATA:
      _strip.SetPixelColor(Pins::STATUS_LED, on ? Colors::CONNECTED_NO_DATA : Colors::BLACK);
      break;
    case DISCONNECTED:
      _strip.SetPixelColor(Pins::STATUS_LED, on ? Colors::DISCONNECTED : Colors::BLACK);
      break;
    }

    _strip.Show();
  }

  void clearStrip()
  {
    for (int i = 0; i < Airports::COUNT; i++)
    {
      _strip.SetPixelColor(i, Colors::BLACK);
    }
  }
}