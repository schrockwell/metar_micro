#ifndef FAA_H
#define FAA_H

#include <Arduino.h>

namespace FAA
{
    bool fetchMETARs(const String airportIDs[], const int numberOfAirports, String &metars);
    String buildURL(const String baseURL, const String airportIDs[], const int numberOfAirports);
}

#endif