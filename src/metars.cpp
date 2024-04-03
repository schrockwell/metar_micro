#include "metars.h"

String parseAirportID(String metar)
{
  // Find the first space
  int spaceIndex = metar.indexOf(' ');
  if (spaceIndex == -1)
  {
    return "";
  }

  // Extract the airport ID
  return metar.substring(0, spaceIndex);
}

category_t parseFlightCategory(String metar)
{
  // Split METAR into words
  String words[20]; // Assuming a METAR won't have more than 20 words
  int wordCount = 0;
  int startIndex = 0;
  for (int i = 0; i < metar.length(); i++)
  {
    if (metar[i] == ' ')
    {
      words[wordCount++] = metar.substring(startIndex, i);
      startIndex = i + 1;
    }
    else if (i == metar.length() - 1)
    {
      words[wordCount++] = metar.substring(startIndex);
    }

    // Ignore excessive words
    if (wordCount >= 20)
    {
      break;
    }
  }

  // Extract information
  int visibility = -1;
  int ceiling = 10000; // Default to high value if not found
  for (int i = 0; i < wordCount; i++)
  {
    // Check if the word contains "SM" for visibility
    if (words[i].endsWith("SM"))
    {
      if (words[i].indexOf("/") == -1)
      {
        // Visibility is in statute miles
        String substr = words[i].substring(0, words[i].length() - 2);
        visibility = substr.toInt();
      }
      else
      {
        // Visibility is in fractions of a mile
        visibility = 0;
      }
    }

    // Check for ceiling information
    if (words[i].startsWith("BKN") || words[i].startsWith("OVC"))
    {
      ceiling = min(ceiling, words[i].substring(3).toInt() * 100);
    }
  }

  return determineFlightCategory(visibility, ceiling);
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
