#ifndef QUERY3_H
#define QUERY3_H

#include <stdio.h>
#include <stdbool.h>
#include "dataset.h"
#include "flights_manager.h"
#include "airports_manager.h"
#include "flights.h"
#include "airports.h"

void q3(const char *start_date, const char *end_date,
        FlightsManager flights, AirportsManager airports, FILE *output);

#endif
