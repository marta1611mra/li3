#ifndef QUERIE3_H
#define QUERIE3_H

#include <stdio.h>
#include <stdbool.h>
#include <glib.h>

#include "flights.h"
#include "airports.h"


void q3(const char *start_date,
        const char *end_date,
        Flights *flights,
        Airports *airports,
        FILE *output);

bool date_in_range(const char* date,
                   const char* start_date,
                   const char* end_date);

Airport* get_airport_by_code(Airports* airports,
                             const char* code);

#endif 
