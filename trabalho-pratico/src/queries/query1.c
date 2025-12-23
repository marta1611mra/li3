#include <stdio.h>
#include <string.h>
#include <glib.h>
#include "query1.h"
#include "airports.h"
#include "airports_manager.h"

// Executa a Query 1.
Q1Result q1(AirportsManager am, const char *code) {
    Q1Result r = { .found = false };

    if (!am || !code) return r;

    Airport a = airports_manager_get(am, code);
    if (!a) return r;

    r.found = true;
    strcpy(r.code, get_airport_code(a));
    strcpy(r.name, get_airport_name(a));
    strcpy(r.city, get_airport_city(a));
    strcpy(r.country, get_airport_country(a));
    strcpy(r.type, get_airport_type(a));

    return r;
}
