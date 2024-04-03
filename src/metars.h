#ifndef METARS_H
#define METARS_H

#include <Arduino.h>

#include "types.h"

namespace METARS
{
    String parseAirportID(String metar);
    category_t parseFlightCategory(String metar);
    category_t determineFlightCategory(int visibility, int ceiling);
}

#endif