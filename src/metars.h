#ifndef METARS_H
#define METARS_H

#include <Arduino.h>

#include "types.h"

namespace METARS
{
    category_t determineFlightCategory(int visibility, int ceiling);

    void parseMETARs(String payload, metar_t *metars, int metarCount);
    void parseMETAR(String metarString, metar_t *metars, int metarCount);
}

#endif