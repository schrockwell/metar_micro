#ifndef AIRPORTS_H
#define AIRPORTS_H

#include <Arduino.h>

const String AIRPORTS[] = {
    "KBDL",
    "",
    "KHFD",
    "KORD",
    "KMCO"};

const int AIRPORT_COUNT = sizeof(AIRPORTS) / sizeof(AIRPORTS[0]);

#endif