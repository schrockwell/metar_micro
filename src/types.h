#ifndef TYPES_H
#define TYPES_H

#include <Arduino.h>

#include "board.h"

enum category_t
{
    NA,
    VFR,
    MVFR,
    IFR,
    LIFR,
};

enum status_t
{
    INITIALIZING,
    CONNECTED_NO_DATA,
    CONNECTED_WITH_DATA,
    DISCONNECTED,
    WIFI_SETUP
};

enum data_source_t
{
    API_SOURCE,
    FAA_SOURCE
};

struct metar_t
{
    const char *airportID;
    int visibility;
    int ceiling;
    category_t category;
    bool lightning;
    int ledIndex;
    int wind;
    int windGust;
    bool fetched;
};

struct inputs_t
{
    bool wifiSetup;

    float ldr;
};

struct settings_t
{
    char ssid[100];
    char password[100];
    bool lightning;
    bool wind;
    int windy_kts;
    int brightness;
};

struct system_t
{
    status_t status = INITIALIZING;
    inputs_t inputs;
    metar_t metars[Airports::COUNT];
    int metarCount;
    settings_t settings;
};

#endif