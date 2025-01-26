#include <Arduino.h>
#include <NeoPixelBus.h>
#include <WiFi.h>

#include "board.h"
#include "commands.h"
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
  Commands::loop();
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
        if (Debug::PRINT_METARS)
        {
          printMetars();
        }
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

  void fetchAsap()
  {
    _retryFetchAfter = 0;
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
      toggleSetup();
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
    Serial.println("Entering setup mode");
    setStatus(WIFI_SETUP);
    WifiSetup::begin();
  }

  void endSetup()
  {
    Serial.println("Exiting setup mode");
    LEDs::clearStrip();
    setStatus(INITIALIZING);
    WifiSetup::end();
    system.settings = Secrets::readSettings();
    _retryFetchAfter = 0;
  }

  void toggleSetup()
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

  void loopWifiSetup()
  {
    if (system.status == WIFI_SETUP)
    {
      WifiSetup::loop();
    }
  }

  void printMetars()
  {
    for (int i = 0; i < system.metarCount; i++)
    {
      metar_t metar = system.metars[i];

      Serial.print(metar.airportID);
      Serial.print(": ");

      if (metar.fetched)
      {
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
        case UNKNOWN:
          Serial.print("UNK");
          break;
        }
      }
      else
      {
        Serial.println("Not fetched");
        continue;
      }

      Serial.print(" (" + String(metar.visibility) + "mi, " +
                   String(metar.ceiling) + "ft, ");

      if (metar.windGust == 0)
      {
        Serial.print(String(metar.wind) + "kt");
      }
      else
      {
        Serial.print(String(metar.wind) + "-" + String(metar.windGust) + "kt");
      }

      if (metar.lightning)
      {
        Serial.print(", lightning");
      }

      Serial.println(")");
    }

    int fetchedCount = 0;
    for (int i = 0; i < system.metarCount; i++)
    {
      metar_t metar = system.metars[i];
      if (metar.fetched)
      {
        fetchedCount++;
      }
    }

    Serial.println("Fetched " + String(fetchedCount) + "/" + String(system.metarCount) + " METARs");

    Serial.print("Not fetched: ");
    for (int i = 0; i < system.metarCount; i++)
    {
      metar_t metar = system.metars[i];
      if (!metar.fetched)
      {
        Serial.print(metar.airportID);
        Serial.print(" ");
      }
    }
    Serial.println();

    Serial.print("Unknown category: ");
    for (int i = 0; i < system.metarCount; i++)
    {
      metar_t metar = system.metars[i];
      if (metar.fetched && metar.category == UNKNOWN)
      {
        Serial.print(metar.airportID);
        Serial.print(" ");
      }
    }
    Serial.println();

    Serial.println("-----------------------------------");
  }
}