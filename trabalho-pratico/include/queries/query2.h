#ifndef QUERY2_H
#define QUERY2_H

#include <stdio.h>
#include "managers/flights_manager.h"
#include "managers/aircrafts_manager.h"

// Executes query 2: Top N aircrafts with most flights (optionally filtered by manufacturer)
void query2_topN_aircrafts(FlightsManager fm,
                           AircraftsManager am,
                           int N,
                           const char *filter_manufacturer,
                           FILE *out);

#endif