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
  Main::loopAnimate();
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

  void loopAnimate()
  {
    if (_status != CONNECTED_WITH_DATA)
    {
      return;
    }

    // Lightning flashes
    for (int i = 0; i < _metarCount; i++)
    {
      metar_t metar = _metars[i];
      if (!metar.lightning || metar.category == NA)
      {
        continue;
      }

      bool flash = random(100) < 5;
      if (flash)
      {
        _strip.SetPixelColor(metar.ledIndex, Colors::LIGHTNING_FLASH);
      }
      else
      {
        _strip.SetPixelColor(metar.ledIndex, getCategoryColor(metar.category));
      }
    }

    _strip.Show();

    // 10 FPS
    delay(100);
  }

  RgbColor getCategoryColor(const category_t category)
  {
    switch (category)
    {
    case VFR:
      return Colors::VFR;
    case MVFR:
      return Colors::MVFR;
    case IFR:
      return Colors::IFR;
    case LIFR:
      return Colors::LIFR;
    case NA:
      return Colors::BLACK;
    }

    return Colors::BLACK;
  }

  void displayMETARs()
  {
    for (int i = 0; i < _metarCount; i++)
    {
      metar_t metar = _metars[i];
      Serial.print(metar.airportID + ": ");
      category_t flightCategory = metar.category;
      RgbColor color = getCategoryColor(flightCategory);
      int ledIndex = metar.ledIndex;

      _strip.SetPixelColor(ledIndex, color);

      switch (flightCategory)
      {
      case VFR:
        Serial.print("VFR");
        break;
      case MVFR:
        Serial.print("MVFR");
        break;
      case IFR:
        Serial.print("IFR");
        break;
      case LIFR:
        Serial.print("LIFR");
        break;
      case NA:
        Serial.print("N/A");
        break;
      }

      Serial.print(" (" + String(metar.visibility) + " mi, " + String(metar.ceiling) + " ft");

      if (metar.lightning)
      {
        Serial.print(", lightning");
      }

      Serial.println(")");
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