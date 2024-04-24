#ifndef API_H

#include <Arduino.h>
#include "types.h"

namespace API
{
    bool fetchMETARs(metar_t *metars, const int metarCount);
    String buildURL(const String baseURL, const uint64_t serial);
}

#endif // API_H