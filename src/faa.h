#ifndef FAA_H
#define FAA_H

#include <Arduino.h>

#include "types.h"

namespace FAA
{
    bool fetchMETARs(data_source_t source, metar_t *metars, const int metarCount);
    String buildFAAURL(const metar_t *metars, const int metarCount);
    String buildAPIURL();
}

#endif