#include <Arduino.h>
#include <NeoPixelBus.h>
#include <WiFi.h>

#include "airports.h"
#include "board.h"
#include "constants.h"
#include "inputs.h"
#include "faa.h"
#include "leds.h"
#include "main.h"
#include "metars.h"
#include "secrets.h"
#include "util.h"
#include "wifi_setup.h"

void setup()
{
  delay(5000);
  Serial.begin(9600);

  Secrets::setup();
  WifiSetup::setup();
  Main::setupSettings();
  Main::setupMETARs();
  LEDs::setupStrip();
  Inputs::setup();

  Main::loopInputs();
  LEDs::loopRedraw();

  Serial.println("Serial number: " + Secrets::getSerialString());
}

void loop()
{
  Main::loopInputs();
  Main::loopMETARFetch();
  LEDs::loopRedraw();
  Main::loopWifiSetup();
}

namespace Main
{
  void setupSettings()
  {
    system.settings = Secrets::readSettings();
  }

  void setupMETARs()
  {
    int metarIndex = 0;
    for (int i = 0; i < Airports::COUNT; i++)
    {
      if (Airports::IDs[i] == "")
      {
        continue;
      }

      system.metars[metarIndex].airportID = Airports::IDs[i];
      system.metars[metarIndex].ledIndex = i;
      metarIndex++;
    }

    system.metarCount = metarIndex;
  }

  void loopMETARFetch()
  {

    if (millis() < _retryFetchAfter)
    {
      return;
    }

    if (system.status == WIFI_SETUP)
    {
      return;
    }

    if (WiFi.status() == WL_CONNECTED)
    {
      if (FAA::fetchMETARs(data_source_t::FAA_SOURCE, system.metars, system.metarCount))
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
      settings_t settings = Secrets::readSettings();
      if (strcmp(settings.ssid, "") == 0)
      {
        Serial.println("No WiFi settings found");
        _retryFetchAfter = millis() + Config::WIFI_RETRY_INTERVAL;
      }
      else
      {
        Serial.println("SSID: " + String(settings.ssid));
        Serial.println("Password: " + String(settings.password));
        WiFi.begin(settings.ssid, settings.password);
        _retryFetchAfter = millis(); // Retry immediately
      }
    }
  }

  void setStatus(status_t status)
  {
    system.status = status;
    LEDs::setStatus(status);
  }

  void loopInputs()
  {
    inputs_t prevInputs = system.inputs;
    system.inputs = Inputs::read();

    // Long-press on setup button => toggle setup mode
    if (system.inputs.wifiSetup && system.inputs.wifiSetup != prevInputs.wifiSetup)
    {
      if (system.status == WIFI_SETUP)
      {
        endSetup();
      }
      else
      {
        beginSetup();
      }
    }

    if (Debug::PRINT_INPUTS)
    {
      if (system.inputs.wifiSetup != prevInputs.wifiSetup)
      {
        Serial.println("WiFi setup: " + String(system.inputs.wifiSetup));
      }

      if (system.inputs.ldr != prevInputs.ldr)
      {
        Serial.println("LDR: " + String(system.inputs.ldr));
      }
    }
  }

  void beginSetup()
  {
    setStatus(WIFI_SETUP);
    WifiSetup::begin();
  }

  void endSetup()
  {
    WifiSetup::end();
    system.settings = Secrets::readSettings();
    LEDs::clearStrip();
    setStatus(INITIALIZING);
    _retryFetchAfter = 0;
  }

  void loopWifiSetup()
  {
    if (system.status == WIFI_SETUP)
    {
      WifiSetup::loop();
    }
  }

  void printMetars()
  {
    if (!Debug::PRINT_METARS)
    {
      return;
    }

    for (int i = 0; i < system.metarCount; i++)
    {
      metar_t metar = system.metars[i];

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

}