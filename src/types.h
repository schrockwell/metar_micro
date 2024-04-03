#ifndef TYPES_H
#define TYPES_H

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
    NEW,
    INITIALIZING,
    CONNECTED_NO_DATA,
    CONNECTED_WITH_DATA,
    DISCONNECTED
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
};

#endif