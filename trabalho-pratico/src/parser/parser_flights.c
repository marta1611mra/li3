#include "parser/parser.h"
#include "dataset.h"
#include "syntactic_validation.h"
#include "logical_validation.h"
#include "flights.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <glib.h>



// Lê e processa o ficheiro flights.csv.
void parse_flights(Dataset d, const char *data_path) {
    char clean_path[512];
    strcpy(clean_path, data_path);
    size_t len = strlen(clean_path);
    while (len > 0 && clean_path[len - 1] == '/') {
    clean_path[len - 1] = '\0';
    len--;
}

    char path[1024];
    snprintf(path, sizeof(path), "%s/flights.csv", clean_path);

    FILE *f = fopen(path, "r");
    if (!f) { perror(path); return; }
    printf("A ler ficheiro: %s\n", path);

    exist_result();
    FILE *ferror = fopen("resultados/flights_errors.csv", "w");
    if (!ferror) { perror("resultados/flights_errors.csv"); fclose(f); return; }

    wcsv_header(f, ferror);

    char line[1500];
    while (fgets(line, sizeof(line), f)) {
        char flight_id[9] = "", departure[20] = "", actual_departure[20] = "", arrival[20] = "", actual_arrival[20] = "";
        char gate[4] = "", origin[5] = "", destination[5] = "", aircraft_id[16] = "", airline[41] = "", tracking_url[101] = "", status_str[16] = "";

        int n = sscanf(line,
                "\"%8[^\"]\",\"%19[^\"]\",\"%19[^\"]\",\"%19[^\"]\",\"%19[^\"]\",\"%3[^\"]\",\"%15[^\"]\",\"%4[^\"]\",\"%4[^\"]\",\"%15[^\"]\",\"%40[^\"]\",\"%100[^\"]\"",
                flight_id, departure, actual_departure, arrival, actual_arrival,
                gate, status_str, origin, destination, aircraft_id, airline, tracking_url);


        if (n < 12) { fprintf(ferror, "%s", line); continue; }

        remove_quotes(flight_id); remove_spc(flight_id);
        remove_quotes(aircraft_id); remove_spc(aircraft_id);
        remove_quotes(status_str); remove_spc(status_str);
        remove_quotes(origin); remove_spc(origin);
        remove_quotes(destination); remove_spc(destination);
        flight_status status;

        // Aceita "On Time" e "OnTime"
        if (strcmp(status_str, "On Time") == 0 || strcmp(status_str, "OnTime") == 0)
        status = OnTime;
        else if (strcmp(status_str, "Delayed") == 0)
        status = Delayed;
        else if (strcmp(status_str, "Cancelled") == 0)
        status = Cancelled;
        else {
        fprintf(ferror, "%s", line);
        continue;
}

    // Validação sintática e lógica
    bool synt_ok =
    validate_flight_id(flight_id) &&
    validate_datetime(departure) &&
    validate_datetime(arrival) &&
    validate_airport_code(origin) &&
    validate_airport_code(destination) &&
    validate_aircraft(aircraft_id, dataset_get_aircrafts(d)) &&
    validate_destination(origin, destination);

    if (status == Cancelled) {
    // "Cancelled" → actual_* devem ser "N/A"
    synt_ok = synt_ok &&
              strcmp(actual_departure, "N/A") == 0 &&
              strcmp(actual_arrival, "N/A") == 0;
    } else {
    synt_ok = synt_ok &&
              validate_datetime(actual_departure) &&
              validate_datetime(actual_arrival);
    }

    if (!synt_ok ||
    !validate_arrival(departure, actual_departure, arrival, actual_arrival, status) ||
    !validate_status(status, actual_departure, actual_arrival)) {
    fprintf(ferror, "%s", line);
    continue;
    }
        
    Flight fv = create_flight(flight_id, departure, actual_departure, arrival,
                                  actual_arrival, gate, status, origin,
                                  destination, aircraft_id, airline, tracking_url);
    if (!fv) {
    fprintf(stderr, "create_flight failed\n");
    fprintf(ferror, "%s", line);
    continue;
    }
    FlightsManager fm = dataset_get_flights(d);
    flights_manager_add(fm, fv);
    }

    fclose(f);
    fclose(ferror);
}