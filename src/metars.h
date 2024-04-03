#ifndef METARS_H
#define METARS_H

#include <Arduino.h>

#include "constants.h"

String parseAirportID(String metar);
category_t parseFlightCategory(String metar);
category_t determineFlightCategory(int visibility, int ceiling);

#endif