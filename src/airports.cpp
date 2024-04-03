#include "airports.h"

namespace Airports
{
    int findAirportIndex(String airportID)
    {
        if (airportID == "")
        {
            return -1;
        }

        for (int i = 0; i < COUNT; i++)
        {
            if (IDs[i] == airportID)
            {
                return i;
            }
        }
        return -1;
    }
}