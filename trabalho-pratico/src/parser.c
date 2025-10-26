#include "parser.h"
#include "syntactic_validation.h"
#include "logical_validation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

struct dataset {
    GList *airports; 
    GList *flights;
    GList *aircrafts;
    GList *passengers;
    GList *reservations;
};

Dataset create_dataset() {
    Dataset d = malloc(sizeof(*d));
    if (!d) {
        fprintf(stderr, "ERROR: INSUFFICIENT MEMORY TO CREATE Dataset.\n");
        exit(EXIT_FAILURE);
    }

    d->airports     = NULL;
    d->flights      = NULL;
    d->aircrafts    = NULL;
    d->passengers   = NULL;
    d->reservations = NULL;
    return d;
}

void destroy_dataset(Dataset d) {
    if (!d) return;

    g_list_free_full(d->airports, (GDestroyNotify)destroy_airport);
    g_list_free_full(d->flights, (GDestroyNotify)destroy_flight);
    g_list_free_full(d->aircrafts, (GDestroyNotify)destroy_aircraft);
    g_list_free_full(d->passengers, (GDestroyNotify)destroy_passenger);
    g_list_free_full(d->reservations, (GDestroyNotify)destroy_reservation);
}

// Removes quotation marks 
static void remove_quotes(char *str) {
    size_t len = strlen(str);
    if (len == 0) return;

    if (str[0] == '"') {
        memmove(str, str + 1, len);
        len--;
    }
    if (len > 0 && str[len - 1] == '"') {
        str[len - 1] = '\0';
    }
}

// Removes spaces and newline characters
static void remove_spc(char *str) {
    int len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == ' ')) {
        str[--len] = '\0';
    }
}

void parse_airports(Dataset d, const char *dataset_path) {
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%s/airports.csv", dataset_path);

    FILE *f = fopen(filepath, "r");
    if (!f) {
        perror("ERROR: COULDN'T OPEN airports.csv");
        return;
    }

    FILE *ferr = fopen("resultados/airports_errors.csv", "w");
    if (!ferr) {
        perror("ERROR: COUNDN'T CREATE airports_errors.csv");
        fclose(f);
        return;
    }

    char line[512];
    char header[512];
    fgets(header, sizeof(header), f);
    fprintf(ferr, "%s", header); //copies header to the error file

    while (fgets(line, sizeof(line), f)) {
        char code[10], name [100], city[100], country[50];
        char lat_str[30], long_str[30], icao[10], type[30];

        int n = sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^\n]",
                       code, name, city, country, lat_str, long_str, icao, type);

        if (n != 8) {
            fprintf(ferr, "%s", line);
            continue;
        }

        remove_quotes(code); remove_spc(code);
        remove_quotes(name); remove_spc(name);
        remove_quotes(city); remove_spc(city);
        remove_quotes(country); remove_spc(country);
        remove_quotes(lat_str); remove_spc(lat_str);
        remove_quotes(long_str); remove_spc(long_str);
        remove_quotes(icao); remove_spc(icao);
        remove_quotes(type); remove_spc(type);

        // validate data
        if (!validate_airport_code(code) ||
            !validate_airport_name(name) ||
            !validate_latitude_longitude(lat_str, long_str)) {
            fprintf(ferr, "%s", line);
            continue;
        }

        // Converts latitude/longitude
        double latitude = strtod(lat_str, NULL);
        double longitude = strtod(long_str, NULL);

        // Create entitie and add to the list
        Airport a = create_airport(code, name, city, country, latitude, longitude, icao, type);
        d->airports = g_list_append(d->airports, a);
    }

    fclose(f);
    fclose(ferr);
}