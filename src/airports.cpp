#include "airports.h"
#include "board.h"

namespace Airports
{
    int findAirportIndex(String airportID)
    {
        if (airportID == "")
        {
            return -1;
        }

        for (int i = 0; i < Airports::COUNT; i++)
        {
            if (Airports::IDs[i] == airportID)
            {
                return i;
            }
        }
        return -1;
    }
}