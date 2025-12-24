#include <stdio.h>
#include <string.h>
#include <glib.h>
#include "query1.h"
#include "airports.h"
#include "airports_manager.h"
#include "output_format.h"

// Executa a Query 1.
void q1(AirportsManager am, const char *code,FILE *out) {


    if (!am || !code || !out) return;

    Airport a = airports_manager_get(am, code);
    if (!a) return;

    if (a) {
        char sep = get_output_separator();
        fprintf(out, "%s%c%s%c%s%c%s%c%s\n",
                get_airport_code(a), sep,
                get_airport_name(a), sep,
                get_airport_city(a), sep,
                get_airport_country(a), sep,
                get_airport_type(a), sep,
                airports_manager_get_arrivals(am, code), sep,
                airports_manager_get_departures(am, code)
            );
    } else {
        fprintf(out, "\n");
    }

}
