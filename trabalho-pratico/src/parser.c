#include "parser.h"
#include "syntactic_validation.h"
#include "logical_validation.h"
#include "flights.c"
#include "reservations.c"
#include "passengers.c"
#include "aircrafts.c"
#include "airports.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

struct dataset {
    GHashTable *airports;
    GHashTable *flights;
    GHashTable *aircrafts;
    GHashTable *passengers;
    GHashTable *reservations;
};

Dataset create_dataset() {
    Dataset d = malloc(sizeof(*d));
    if (!d) {
        fprintf(stderr, "ERROR: INSUFFICIENT MEMORY TO CREATE Dataset.\n");
        exit(EXIT_FAILURE);
    }

    d->airports     = g_hash_table_new_full(g_str_hash, g_str_equal, free, (GDestroyNotify)destroy_airport);
    d->flights      = g_hash_table_new_full(g_str_hash, g_str_equal, free, (GDestroyNotify)destroy_flight);
    d->aircrafts    = g_hash_table_new_full(g_str_hash, g_str_equal, free, (GDestroyNotify)destroy_aircraft);
    d->passengers   = g_hash_table_new_full(g_str_hash, g_str_equal, free, (GDestroyNotify)destroy_passenger);
    d->reservations = g_hash_table_new_full(g_str_hash, g_str_equal, free, (GDestroyNotify)destroy_reservation);

    return d;
}

void destroy_dataset(Dataset d) {
    if (!d) return;
    g_hash_table_destroy(d->airports);
    g_hash_table_destroy(d->flights);
    g_hash_table_destroy(d->aircrafts);
    g_hash_table_destroy(d->passengers);
    g_hash_table_destroy(d->reservations);
    free(d);
}



// Removes quotation marks 
static void remove_quotes(char *str) {
    size_t len = strlen(str);
    if (len == 0) return;

    if (str[0] == '"') memmove(str, str + 1, len);
    len = strlen(str);
    if (len > 0 && str[len - 1] == '"') str[len - 1] = '\0';
}

static void remove_spc(char *str) {
    int len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == ' ' || str[len - 1] == '\r'))
        str[--len] = '\0';
}

void parse_airports(Dataset d, const char *dataset_path) {
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%s/airports.csv", dataset_path);

    FILE *f = fopen(filepath, "r");
    if (!f) { perror("airports.csv"); return; }

    system("mkdir -p resultados");
    FILE *ferr = fopen("resultados/airports_errors.csv", "w");
    if (!ferr) { perror("airports_errors.csv"); fclose(f); return; }

    char line[512], header[512];
    fgets(header, sizeof(header), f);
    fprintf(ferr, "%s", header);

    while (fgets(line, sizeof(line), f)) {
        char code[10], name[100], city[100], country[50];
        char lat_str[30], long_str[30], icao[10], type[30];

        int n = sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^\n]",
                       code, name, city, country, lat_str, long_str, icao, type);
        if (n != 8) { fprintf(ferr, "%s", line); continue; }

        remove_quotes(code); remove_spc(code);
        remove_quotes(name); remove_spc(name);
        remove_quotes(city); remove_spc(city);
        remove_quotes(country); remove_spc(country);
        remove_quotes(lat_str); remove_spc(lat_str);
        remove_quotes(long_str); remove_spc(long_str);
        remove_quotes(icao); remove_spc(icao);
        remove_quotes(type); remove_spc(type);

        if (!validate_airport_code(code) ||
            !validate_latitude_longitude(lat_str, long_str)) {
            fprintf(ferr, "%s", line);
            continue;
        }

        double latitude = strtod(lat_str, NULL);
        double longitude = strtod(long_str, NULL);

        Airport a = create_airport(code, name, city, country, latitude, longitude, icao, type);
        g_hash_table_insert(d->airports, strdup(a->code), a);
    }

    fclose(f);
    fclose(ferr);
}




void parse_aircrafts(Dataset d, const char *dataset_path) {
    char path[256];
    snprintf(path, sizeof(path), "%s/aircrafts.csv", dataset_path);

    FILE *f = fopen(path, "r");
    if (!f) { perror("aircrafts.csv"); return; }

    system("mkdir -p resultados");
    FILE *ferr = fopen("resultados/aircrafts_errors.csv", "w");
    if (!ferr) { perror("aircrafts_errors.csv"); fclose(f); return; }

    char line[512], header[512];
    fgets(header, sizeof(header), f);
    fprintf(ferr, "%s", header);

    while (fgets(line, sizeof(line), f)) {
        char id[15], manufacturer[50], model[50];
        char year_str[10], capacity_str[10], range_str[10];

        int n = sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^,],%[^\n]",
                       id, manufacturer, model, year_str, capacity_str, range_str);
        if (n != 6) { fprintf(ferr, "%s", line); continue; }

        remove_quotes(id); remove_spc(id);
        remove_quotes(manufacturer); remove_spc(manufacturer);
        remove_quotes(model); remove_spc(model);
        remove_quotes(year_str); remove_spc(year_str);
        remove_quotes(capacity_str); remove_spc(capacity_str);
        remove_quotes(range_str); remove_spc(range_str);

        int year = atoi(year_str);
        int capacity = atoi(capacity_str);
        int range = atoi(range_str);

        if (strlen(id) == 0 || year <= 0 || capacity <= 0 || range <= 0) {
            fprintf(ferr, "%s", line);
            continue;
        }

        Aircraft a = create_aircraft(id, manufacturer, model, year, capacity, range);
        g_hash_table_insert(d->aircrafts, strdup(a->id), a);
    }

    fclose(f);
    fclose(ferr);
}

void parse_flights(Dataset d, const char *dataset_path) {
    char path[256];
    snprintf(path, sizeof(path), "%s/flights.csv", dataset_path);

    FILE *f = fopen(path, "r");
    if (!f) { perror("flights.csv"); return; }

    system("mkdir -p resultados");
    FILE *ferr = fopen("resultados/flights_errors.csv", "w");
    if (!ferr) { perror("flights_errors.csv"); fclose(f); return; }

    char line[512], header[512];
    fgets(header, sizeof(header), f);
    fprintf(ferr, "%s", header);

    while (fgets(line, sizeof(line), f)) {
        char flight_id[8], departure[19], actual_departure[19], arrival[19], actual_arrival[19];
        char gate[3], origin[4], destination[4], aircraft_id[15], airline[40], tracking_url[100], status_str[15];

        int n = sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^\n]",
                       flight_id, departure, actual_departure, arrival, actual_arrival,
                       gate, status_str, origin, destination, aircraft_id, airline, tracking_url);

        if (n != 12) { fprintf(ferr, "%s", line); continue; }

        remove_quotes(flight_id); remove_spc(flight_id);
        remove_quotes(aircraft_id); remove_spc(aircraft_id);
        remove_quotes(status_str); remove_spc(status_str);
        remove_quotes(origin); remove_spc(origin);
        remove_quotes(destination); remove_spc(destination);

        flight_status status;
        if (strcmp(status_str, "OnTime") == 0) status = OnTime;
        else if (strcmp(status_str, "Delayed") == 0) status = Delayed;
        else if (strcmp(status_str, "Cancelled") == 0) status = Cancelled;
        else { fprintf(ferr, "%s", line); continue; }

        struct flight ftmp;
        strcpy(ftmp.flight_id, flight_id);
        strcpy(ftmp.aircraft_id, aircraft_id);
        strcpy(ftmp.origin, origin);
        strcpy(ftmp.destination, destination);
        ftmp.status = status;

        if (!validate_aircraft(ftmp, d->aircrafts)) {
            fprintf(ferr, "%s", line);
            continue;
        }

        Flight fv = create_flight(flight_id, departure, actual_departure, arrival,
                                  actual_arrival, gate, status, origin,
                                  destination, aircraft_id, airline, tracking_url);

        g_hash_table_insert(d->flights, strdup(fv->flight_id), fv);
    }

    fclose(f);
    fclose(ferr);
}

void parse_passengers(Dataset d, const char *dataset_path) {
    char path[256];
    snprintf(path, sizeof(path), "%s/passengers.csv", dataset_path);

    FILE *f = fopen(path, "r");
    if (!f) { perror("passengers.csv"); return; }

    system("mkdir -p resultados");
    FILE *ferr = fopen("resultados/passengers_errors.csv", "w");
    if (!ferr) { perror("passengers_errors.csv"); fclose(f); return; }

    char line[512], header[512];
    fgets(header, sizeof(header), f);
    fprintf(ferr, "%s", header);

    while (fgets(line, sizeof(line), f)) {
        char document_id[10], first_name[30], last_name[30], dob[11];
        char nationality[20], gender[10], email[50], phone[15], address[50], photo[1000];

        int n = sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^\n]",
                       document_id, first_name, last_name, dob, nationality,
                       gender, email, phone, address, photo);

        if (n != 10) { fprintf(ferr, "%s", line); continue; }

        if (!validate_email(email) ||
            !validate_document_number(document_id) ||
            !validate_gender(gender) ||
            !validate_date(dob)) {
            fprintf(ferr, "%s", line);
            continue;
        }

        Passenger p = create_passenger(document_id, first_name, last_name, dob,
                                       nationality, gender, email, phone, address, photo);
        g_hash_table_insert(d->passengers, strdup(p->document_id), p);
    }

    fclose(f);
    fclose(ferr);
}


void parse_reservations(Dataset d, const char *dataset_path) {
    char path[256];
    snprintf(path, sizeof(path), "%s/reservations.csv", dataset_path);

    FILE *f = fopen(path, "r");
    if (!f) { perror("reservations.csv"); return; }

    system("mkdir -p resultados");
    FILE *ferr = fopen("resultados/reservations_errors.csv", "w");
    if (!ferr) { perror("reservations_errors.csv"); fclose(f); return; }

    char line[512], header[512];
    fgets(header, sizeof(header), f);
    fprintf(ferr, "%s", header);

    while (fgets(line, sizeof(line), f)) {
        char reservation_id[20], flight_id[2][10], document_number[15];
        char qr_code[50];
        int seat[2], extra_luggage[2], priority_boarding[2];
        double price[2];

        int n = sscanf(line, "%[^,],%[^,],%[^,],%[^,],%d,%d,%lf,%lf,%d,%d,%d,%d,%[^\n]",
                       reservation_id, flight_id[0], flight_id[1], document_number,
                       &seat[0], &seat[1], &price[0], &price[1],
                       &extra_luggage[0], &extra_luggage[1],
                       &priority_boarding[0], &priority_boarding[1],
                       qr_code);

        if (n != 13) { fprintf(ferr, "%s", line); continue; }

        if (!g_hash_table_contains(d->passengers, document_number)) {
            fprintf(ferr, "%s", line);
            continue;
        }

        Reservation r = create_reservation(reservation_id, flight_id, document_number,
                                           seat, price, extra_luggage,
                                           priority_boarding, qr_code);
        g_hash_table_insert(d->reservations, strdup(r->reservation_id), r);
    }

    fclose(f);
    fclose(ferr);
}