#ifndef TYPES_H
#define TYPES_H

#include <Arduino.h>

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
    String raw;
    String airportID;
    int visibility;
    int ceiling;
    category_t category;
    bool lightning;
    int ledIndex;
    int wind;
    int windGust;
};

struct inputs_t
{
    bool wifiSetup;

    float ldr;
};

#endif