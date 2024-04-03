#include "metars.h"

namespace METARS
{
  void parseMETARs(String payload, metar_t *metars, int metarCount)
  {
    // Split payload into METARs
    String metarStrings[metarCount];
    int metarIndex = 0;
    int startIndex = 0;
    for (int i = 0; i < payload.length(); i++)
    {
      if (payload[i] == '\n')
      {
        metarStrings[metarIndex++] = payload.substring(startIndex, i);
        startIndex = i + 1;
      }

      // Ignore excessive METARs
      if (metarIndex >= metarCount)
      {
        break;
      }
    }

    // Parse each METAR
    for (int i = 0; i < metarCount; i++)
    {
      parseMETAR(metarStrings[i], metars, metarCount);
    }
  }

  void parseMETAR(String metarString, metar_t *metars, int metarCount)
  {
    // Split METAR into words
    String words[50]; // Assuming a METAR won't have more than 50 words
    int wordCount = 0;
    int startIndex = 0;
    for (int i = 0; i < metarString.length(); i++)
    {
      if (metarString[i] == ' ')
      {
        words[wordCount++] = metarString.substring(startIndex, i);
        startIndex = i + 1;
      }
      else if (i == metarString.length() - 1)
      {
        words[wordCount++] = metarString.substring(startIndex);
      }

      // Ignore excessive words
      if (wordCount >= 50)
      {
        break;
      }
    }

    // Find the metar
    int metarIndex = -1;
    for (int i = 0; i < metarCount; i++)
    {
      if (metars[i].airportID == words[0])
      {
        metarIndex = i;
        break;
      }
    }

    if (metarIndex == -1)
    {
      return;
    }

    // Extract information
    metar_t newMetar = metars[metarIndex];
    newMetar.raw = metarString;
    newMetar.visibility = -1;
    newMetar.ceiling = 10000; // Default to high value if not found

    for (int i = 0; i < wordCount; i++)
    {
      // Check if the word contains "SM" for visibility
      if (words[i].endsWith("SM"))
      {
        if (words[i].indexOf("/") == -1)
        {
          // Visibility is in statute miles
          String substr = words[i].substring(0, words[i].length() - 2);
          newMetar.visibility = substr.toInt();
        }
        else
        {
          int whole = words[i - 1].toInt();
          if (whole <= 10)
          {
            // e.g. "1 3/4 SM" => 1.75 miles
            newMetar.visibility = whole;
          }
          else
          {
            // e.g. "18006KT 1/4SM" => 0.25 miles
            newMetar.visibility = 0;
          }
        }
      }

      // Check for ceiling information
      if (words[i].startsWith("BKN") || words[i].startsWith("OVC"))
      {
        newMetar.ceiling = min(newMetar.ceiling, words[i].substring(3).toInt() * 100);
      }

      if (words[i].startsWith("LTG") || words[i] == "TS")
      {
        newMetar.lightning = true;
      }
    }

    newMetar.category = determineFlightCategory(newMetar.visibility, newMetar.ceiling);

    // Update the array
    metars[metarIndex] = newMetar;
  }

  category_t determineFlightCategory(int visibility, int ceiling)
  {
    if (visibility == -1)
    {
      return NA;
    }
    else if (visibility < 1 || ceiling < 500)
    {
      return LIFR;
    }
    else if (visibility < 3 || ceiling < 1000)
    {
      return IFR;
    }
    else if (visibility < 5 || ceiling < 3000)
    {
      return MVFR;
    }
    else
    {
      return VFR;
    }
  }
}
