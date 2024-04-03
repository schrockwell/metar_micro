#ifndef AIRPORTS_H
#define AIRPORTS_H

#include <Arduino.h>

namespace Airports
{
    const String IDs[] = {
        "KBDL",
        "",
        "KHFD",
        "KORD",
        "KMCO"};

    const int COUNT = sizeof(IDs) / sizeof(IDs[0]);

    int findAirportIndex(String airportID);
}

#endif