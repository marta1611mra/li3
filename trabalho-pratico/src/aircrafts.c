#include <stdio.h>

struct aircraft {
    char id[15]; // identification (tail number)
    char manufacturer[50]; // manufacturer of the aircraft
    char model[50]; // model of the aircraft
    char year[6]; // year of fabrication
    int capacity[50]; // max capacity of the aircraft
    int range[100]; // maximum range in km's
};