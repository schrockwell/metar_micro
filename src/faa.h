#ifndef FAA_H
#define FAA_H

#include <Arduino.h>

#include "types.h"

namespace FAA
{
    bool fetchMETARs(metar_t *metars, const int metarCount);
    String buildURL(const String baseURL, const metar_t *metars, const int metarCount);
}

#endif