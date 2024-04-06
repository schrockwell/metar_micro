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

  // Display the initial status
  Main::loopInputs();
  Main::loopRedraw();

  WiFi.begin(Secrets::SSID, Secrets::PASSWORD);
}

void loop()
{
  Main::loopInputs();
  Main::loopMETARFetch();
  Main::loopRedraw();
}

namespace Main
{
  NeoPixelBus<NeoRgbFeature, NeoWs2812xMethod> _strip(Airports::COUNT, Pins::NEOPIXEL);

  status_t _status = INITIALIZING;
  metar_t _metars[Airports::COUNT];
  inputs_t _inputs;
  bool _forceRedraw = true;
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

    _status = INITIALIZING;
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
        _status = CONNECTED_WITH_DATA;
        _forceRedraw = true;
        retryAfter = millis() + 300000; // Retry every 5 minutes
      }
      else
      {
        Serial.println("WiFi connected, but bad response from FAA");
        _status = CONNECTED_NO_DATA;
        retryAfter = millis() + 60000; // Retry every 60 seconds
      }
    }
    else
    {
      Serial.println("Reconnecting to WiFi...");
      _status = DISCONNECTED;
      WiFi.begin(Secrets::SSID, Secrets::PASSWORD);
      retryAfter = 0; // Retry immediately
    }
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
      Serial.println("LDR: " + String(_inputs.ldr));
    }

    if (_inputs.brightness != prevInputs.brightness)
    {
      Serial.println("Brightness: " + String(_inputs.brightness));
    }

    if (_inputs.contrast != prevInputs.contrast)
    {
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

  void clearStrip()
  {
    for (int i = 0; i < Airports::COUNT; i++)
    {
      _strip.SetPixelColor(i, Colors::BLACK);
    }
  }

  void setStationPixel(int index, RgbColor color)
  {
    float userBrightness = _inputs.brightness;
    float roomDimness = 0.0;

    if (_inputs.autoDimming)
    {
      userBrightness = _inputs.brightness * 1.2; // Give the user 20% extra brightness headroom
      roomDimness = (1.0 - _inputs.ldr) * _inputs.contrast;
    }

    // float userBrightness = _inputs.brightness * 1.2; // Give the user 20% extra brightness headroom
    // float roomDimness = (1.0 - _inputs.ldr) * _inputs.contrast;

    uint8_t brightness = max(5, min(255, (userBrightness - roomDimness) * 255));
    _strip.SetPixelColor(index, color.Dim(brightness));
  }

  void loopRedraw()
  {
    static inputs_t prevInputs = _inputs;
    static status_t prevStatus = _status;
    static unsigned long lastAnimateAt = 0;

    // Redraw all if:
    // - METARs changed (force redraw)
    // - status changed
    // - LDR changed
    // - brightness changed
    // - contrast changed
    // - dimming setting changed
    bool redrawAll = _forceRedraw ||
                     prevStatus != _status ||
                     prevInputs.ldr != _inputs.ldr ||
                     prevInputs.brightness != _inputs.brightness ||
                     prevInputs.contrast != _inputs.contrast ||
                     prevInputs.autoDimming != _inputs.autoDimming;

    if (redrawAll)
    {
      if (_status == CONNECTED_WITH_DATA)
      {
        drawMETARs();
      }
      else
      {
        drawStatus();
      }
    }

    // Animate:
    // - lightning flashes
    // - windy stations
    if (_status == CONNECTED_WITH_DATA)
    {
      if (millis() - lastAnimateAt > 100)
      {
        drawAnimationFrame();
        lastAnimateAt = millis();
      }
    }

    _strip.Show();

    prevInputs = _inputs;
    prevStatus = _status;
    _forceRedraw = false;
  }

  void drawMETARs()
  {
    for (int i = 0; i < _metarCount; i++)
    {
      metar_t metar = _metars[i];

      setStationPixel(metar.ledIndex, getCategoryColor(metar.category));

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

      Serial.print(" (" + String(metar.visibility) + " mi, " +
                   String(metar.ceiling) + " ft, " +
                   String(metar.wind) + " gusting " +
                   String(metar.windGust) + " kt");

      if (metar.lightning)
      {
        Serial.print(", lightning");
      }

      Serial.println(")");
    }
    Serial.println("-----------------------------------");

    _strip.Show();
  }

  void drawAnimationFrame()
  {
    // Windy stations
    for (int i = 0; i < _metarCount; i++)
    {
      metar_t metar = _metars[i];
      if (max(metar.wind, metar.windGust) < 10)
      {
        continue;
      }

      if (_inputs.windVisible)
      {
        // Wind flickering 20% of the frames
        bool flashWind = random(100) < 20;
        if (flashWind)
        {
          RgbColor flickerColor = getCategoryColor(metar.category).Dim(128);
          setStationPixel(metar.ledIndex, flickerColor);
        }
        else
        {
          setStationPixel(metar.ledIndex, getCategoryColor(metar.category));
        }
      }
    }

    if (_inputs.lightningVisible)
    {
      // Lightning flashes 5% of the frames
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
          setStationPixel(metar.ledIndex, Colors::LIGHTNING_FLASH);
        }
        else
        {
          setStationPixel(metar.ledIndex, getCategoryColor(metar.category));
        }
      }
    }
  }

  void drawStatus()
  {
    clearStrip();

    switch (_status)
    {
    case INITIALIZING:
      Serial.println("INITIALIZING");
      setStationPixel(Pins::STATUS_LED, Colors::INITIALIZING);
      break;
    case CONNECTED_NO_DATA:
      Serial.println("CONNECTED_NO_DATA");
      setStationPixel(Pins::STATUS_LED, Colors::CONNECTED_NO_DATA);
      break;
    case DISCONNECTED:
      Serial.println("DISCONNECTED");
      setStationPixel(Pins::STATUS_LED, Colors::DISCONNECTED);
      break;
    }
  }
}