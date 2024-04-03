#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>

#include "constants.h"

void setup();
void loop();

void loopMETARFetch();
String buildURL(const String baseURL, const String airportIDs[], const int numberOfAirports);

void displayMETARS(String lines);
void displayMETAR(String metar);
int findAirportIndex(String airportID);
void setStatusLED(status_t newStatus);
void loopStatusLED();
void clearStrip();

#endif