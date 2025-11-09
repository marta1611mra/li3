#include <stdio.h>
#include <string.h>
#include <glib.h>
#include "query1.h"
#include "airports.h"
#include "airports_manager.h"

void query1(AirportsManager am, const char *code, FILE *out) {
    if (!am || !code || !out) return;

    Airport a = airports_manager_get(am, code);

    if(a) {
        fprintf(out, "%s;%s;%s;%s;%s\n", get_airport_code(a), get_airport_name(a), get_airport_city(a), get_airport_country(a), get_airport_type(a));
    } else {
        fprintf(out, "\n");
    }
}