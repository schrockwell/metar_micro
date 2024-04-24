#include <Arduino.h>
#include <NeoPixelBus.h>
#include <WiFi.h>
#include <LEAmDNS.h>

#include "airports.h"
#include "boards.h"
#include "constants.h"
#include "inputs.h"
#include "faa.h"
#include "main.h"
#include "metars.h"
#include "secrets.h"
#include "util.h"
#include "wifi_setup.h"

void setup()
{
  Serial.begin(115200);

  MDNS.begin("airmap");
  MDNS.addService("http", "tcp", 80);
  MDNS.announce();

  Secrets::setup();
  WifiSetup::setup();
  Main::setupMETARs();
  Main::setupStrip();
  Inputs::setup();

  Main::loopInputs();
  Main::loopRedraw();

  Serial.println("Serial number: " + Secrets::getSerialString());
}

void loop()
{
  MDNS.update();

  Main::loopInputs();
  Main::loopMETARFetch();
  Main::loopRedraw();
  Main::loopWifiSetup();
}

namespace Main
{
  NeoPixelBus<NEOPIXEL_ORDERING, NeoWs2812xMethod> _strip(Airports::COUNT, Pins::NEOPIXEL);

  status_t _status = INITIALIZING;
  metar_t _metars[Airports::COUNT];
  inputs_t _inputs;
  bool _forceRedraw = true;
  int _metarCount;
  unsigned long _retryFetchAfter = 0;

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
  }

  void loopMETARFetch()
  {

    if (millis() < _retryFetchAfter)
    {
      return;
    }

    if (_status == WIFI_SETUP)
    {
      return;
    }

    if (WiFi.status() == WL_CONNECTED)
    {
      if (FAA::fetchMETARs(data_source_t::API_SOURCE, _metars, _metarCount))
      {
        printMetars();
        setStatus(CONNECTED_WITH_DATA);
        _retryFetchAfter = millis() + Config::METAR_FETCH_INTERVAL;
      }
      else
      {
        Serial.println("WiFi connected, but bad response from FAA");
        setStatus(CONNECTED_NO_DATA);
        _retryFetchAfter = millis() + Config::METAR_RETRY_INTERVAL;
      }
    }
    else
    {
      Serial.println("Reconnecting to WiFi...");
      setStatus(DISCONNECTED);
      String ssid;
      String password;
      Secrets::readWiFiCredentials(ssid, password);
      WiFi.begin(ssid.c_str(), password.c_str());
      _retryFetchAfter = millis() + Config::WIFI_RETRY_INTERVAL;
    }
  }

  void loopInputs()
  {
    inputs_t prevInputs = _inputs;
    _inputs = Inputs::read();

    if (_inputs.wifiSetup != prevInputs.wifiSetup)
    {
      if (_inputs.wifiSetup)
      {
        setStatus(WIFI_SETUP);
        WifiSetup::begin();
      }
      else
      {
        WifiSetup::end();
        setStatus(INITIALIZING);
        _retryFetchAfter = 0;
      }
    }

    if (Debug::PRINT_INPUTS)
    {
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

      if (_inputs.maxBrightness != prevInputs.maxBrightness)
      {
        Serial.println("Max brightness: " + String(_inputs.maxBrightness));
      }

      if (_inputs.minBrightness != prevInputs.minBrightness)
      {
        Serial.println("Min brightness: " + String(_inputs.minBrightness));
      }
    }
  }

  void loopWifiSetup()
  {
    if (_status == WIFI_SETUP)
    {
      WifiSetup::loop();
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
    _strip.SetPixelColor(index, color.Dim(getDesiredBrightness()));
  }

  uint8_t getDesiredBrightness()
  {
    float userBrightness = _inputs.maxBrightness;
    float dimming = 0.0;

    if (_inputs.autoDimming)
    {
      // LDR:
      //   LDR high (1.0) -> room is bright -> less LED dimming -> bright LEDs
      //   LDR low (0.0) -> room is dark -> more LED dimming -> dim LEDs
      //
      // Min brightness knob:
      //   Min brightness high (1.0) -> less LED dimming -> bright LEDs
      //   Min brightness low (0.0) -> more LED dimming -> dim LEDs
      dimming = (1.0 - _inputs.ldr) * (1.0 - _inputs.minBrightness);
    }

    return clamp((userBrightness - dimming) * (float)Features::MASTER_MAX_BRIGHTNESS, Features::MASTER_MIN_BRIGHTNESS, Features::MASTER_MAX_BRIGHTNESS);
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
    // - brightness knobs changed
    // - dimming setting changed
    bool redrawAll = _forceRedraw ||
                     prevStatus != _status ||
                     prevInputs.ldr != _inputs.ldr ||
                     prevInputs.maxBrightness != _inputs.maxBrightness ||
                     prevInputs.minBrightness != _inputs.minBrightness ||
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
      if (millis() - lastAnimateAt > Config::ANIMATION_FRAME_DURATION)
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
    }
  }

  void drawAnimationFrame()
  {
    // Windy stations
    for (int i = 0; i < _metarCount; i++)
    {
      metar_t metar = _metars[i];
      if (max(metar.wind, metar.windGust) < Config::GUSTY_WIND_THRESHOLD)
      {
        continue;
      }

      if (_inputs.windVisible)
      {
        bool flicker = random(100) < Config::FLICKER_WIND_PERCENT;
        if (flicker)
        {
          RgbColor flickerColor = getCategoryColor(metar.category).Dim(Config::FLICKER_WIND_DIMMING);
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
      for (int i = 0; i < _metarCount; i++)
      {
        metar_t metar = _metars[i];
        if (!metar.lightning || metar.category == NA)
        {
          continue;
        }

        bool flash = random(100) < Config::FLASH_LIGHTNING_PERCENT;
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
    case WIFI_SETUP:
      Serial.println("WIFI_SETUP");
      setStationPixel(Pins::STATUS_LED, Colors::WIFI_SETUP);
      break;
    }
  }

  void printMetars()
  {
    if (!Debug::PRINT_METARS)
    {
      return;
    }

    for (int i = 0; i < _metarCount; i++)
    {
      metar_t metar = _metars[i];

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
                   String(metar.wind) + " kt ");

      if (metar.windGust > 0)
      {
        Serial.print(" gusting " + String(metar.windGust) + " kt");
      }

      if (metar.lightning)
      {
        Serial.print(", lightning");
      }

      Serial.println(")");
    }

    Serial.println("-----------------------------------");
  }

  void setStatus(status_t status)
  {
    _status = status;
    _forceRedraw = true;
  }
}