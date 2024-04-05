#include <Arduino.h>
#include <NeoPixelBus.h>
#include <WiFi.h>

#include "airports.h"
#include "constants.h"
#include "inputs.h"
#include "faa.h"
#include "main.h"
#include "metars.h"
#include "secrets.h"

void setup()
{
  Serial.begin(115200);

  Main::setupMETARs();
  Main::setupStrip();
  Inputs::setup();

  Main::loopInputs();

  WiFi.begin(Secrets::SSID, Secrets::PASSWORD);
}

void loop()
{
  Main::loopInputs();
  Main::loopMETARFetch();
  Main::loopAnimate();
  Main::loopStatusLED();
}

namespace Main
{
  NeoPixelBus<NeoRgbFeature, NeoWs2812xMethod> _strip(Airports::COUNT, Pins::NEOPIXEL);

  status_t _status = NEW;
  metar_t _metars[Airports::COUNT];
  inputs_t _inputs;
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
        setStatusLED(CONNECTED_WITH_DATA);
        displayMETARs();
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
    unsigned long animatedAt = 0;

    // 10 FPS
    if (millis() - animatedAt < 100)
    {
      return;
    }

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
        setPixel(metar.ledIndex, Colors::LIGHTNING_FLASH);
      }
      else
      {
        setPixel(metar.ledIndex, getCategoryColor(metar.category));
      }
    }

    _strip.Show();

    animatedAt = millis();
  }

  void loopInputs()
  {
    inputs_t prevInputs = _inputs;
    _inputs = Inputs::read();

    if (_inputs.windVisible != prevInputs.windVisible)
    {
      Serial.println("Wind visibility: " + String(_inputs.windVisible));
    }

    if (_inputs.lightningVisible != prevInputs.lightningVisible)
    {
      Serial.println("Lightning visibility: " + String(_inputs.lightningVisible));
    }

    if (_inputs.autoDimming != prevInputs.autoDimming)
    {
      Serial.println("Auto dimming: " + String(_inputs.autoDimming));
    }

    if (_inputs.wifiSetup != prevInputs.wifiSetup)
    {
      Serial.println("WiFi setup: " + String(_inputs.wifiSetup));
    }

    if (_inputs.ldr != prevInputs.ldr)
    {
      displayMETARs();
      Serial.println("LDR: " + String(_inputs.ldr));
    }

    if (_inputs.brightness != prevInputs.brightness)
    {
      displayMETARs();
      Serial.println("Brightness: " + String(_inputs.brightness));
    }

    if (_inputs.contrast != prevInputs.contrast)
    {
      displayMETARs();
      Serial.println("Contrast: " + String(_inputs.contrast));
    }
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
    if (_status != CONNECTED_WITH_DATA)
    {
      return;
    }

    for (int i = 0; i < _metarCount; i++)
    {
      metar_t metar = _metars[i];

      setPixel(metar.ledIndex, getCategoryColor(metar.category));

      Serial.print(metar.airportID + ": ");
      switch (metar.category)
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
      setPixel(Pins::STATUS_LED, Colors::INITIALIZING);
      _strip.Show();
      break;
    case CONNECTED_NO_DATA:
      clearStrip();
      setPixel(Pins::STATUS_LED, Colors::CONNECTED_NO_DATA);
      _strip.Show();
      break;
    case DISCONNECTED:
      clearStrip();
      setPixel(Pins::STATUS_LED, Colors::DISCONNECTED);
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
      setPixel(Pins::STATUS_LED, on ? Colors::INITIALIZING : Colors::BLACK);
      break;
    case CONNECTED_NO_DATA:
      setPixel(Pins::STATUS_LED, on ? Colors::CONNECTED_NO_DATA : Colors::BLACK);
      break;
    case DISCONNECTED:
      setPixel(Pins::STATUS_LED, on ? Colors::DISCONNECTED : Colors::BLACK);
      break;
    }

    _strip.Show();
  }

  void clearStrip()
  {
    for (int i = 0; i < Airports::COUNT; i++)
    {
      setPixel(i, Colors::BLACK);
    }
  }

  void setPixel(int index, RgbColor color)
  {
    float userBrightness = _inputs.brightness * 1.2; // Give the user 20% extra brightness headroom
    float roomDimness = (1.0 - _inputs.ldr) * _inputs.contrast;
    uint8_t brightness = max(10, min(255, (userBrightness - roomDimness) * 255));

    _strip.SetPixelColor(index, color.Dim(brightness));
  }
}