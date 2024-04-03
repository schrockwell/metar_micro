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
  Serial.begin(115200);

  Main::setupMETARs();
  Main::setupStrip();

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
  metar_t _metars[Airports::COUNT];
  int _metarCount;

  void setupMETARs()
  {
    int metarIndex = 0;
    for (int i = 0; i < Airports::COUNT; i++)
    {
      if (Airports::IDs[i] == "")
      {
        continue;
      }

      _metars[metarIndex].airportID = Airports::IDs[i];
      _metars[metarIndex].ledIndex = i;
      metarIndex++;
    }

    _metarCount = metarIndex;
  }

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
      if (FAA::fetchMETARs(_metars, _metarCount))
      {
        displayMETARs();
        setStatusLED(CONNECTED_WITH_DATA);
        retryAfter = millis() + 300000; // Retry every 5 minutes
      }
      else
      {
        Serial.println("WiFi connected, but bad response from FAA");
        setStatusLED(CONNECTED_NO_DATA);
        retryAfter = millis() + 60000; // Retry every 60 seconds
      }
    }
    else
    {
      Serial.println("Reconnecting to WiFi...");
      setStatusLED(DISCONNECTED);
      WiFi.begin(Secrets::SSID, Secrets::PASSWORD);
      retryAfter = 0; // Retry immediately
    }
  }

  void displayMETARs()
  {
    for (int i = 0; i < _metarCount; i++)
    {
      Serial.print(_metars[i].airportID + ": ");
      category_t flightCategory = _metars[i].category;
      int ledIndex = _metars[i].ledIndex;
      switch (flightCategory)
      {
      case VFR:
        Serial.println("VFR");
        _strip.SetPixelColor(ledIndex, Colors::VFR);
        break;
      case MVFR:
        Serial.println("MVFR");
        _strip.SetPixelColor(ledIndex, Colors::MVFR);
        break;
      case IFR:
        Serial.println("IFR");
        _strip.SetPixelColor(ledIndex, Colors::IFR);
        break;
      case LIFR:
        Serial.println("LIFR");
        _strip.SetPixelColor(ledIndex, Colors::LIFR);
        break;
      case NA:
        Serial.println("N/A");
        _strip.SetPixelColor(ledIndex, Colors::BLACK);
        break;
      }
    }
    Serial.println("-----------------------------------");

    _strip.Show();
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