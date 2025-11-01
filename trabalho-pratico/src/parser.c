#include "parser.h"
#include "dataset.h"
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
#include <ctype.h>
#include <glib.h>

// Removes quotation marks 
static void remove_quotes(char *str) {
    if(!str) return;
    size_t len = strlen(str);
    if (len == 0) return;
    if (str[0] == '"') memmove(str, str + 1, len);
    len = strlen(str);
    if (len > 0 && str[len - 1] == '"') str[len - 1] = '\0';
}

static void remove_spc(char *str) {
    if (!str) return;
    int len = (int)strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\t' || str[len - 1] == ' ' || str[len - 1] == '\r')) {
        str[--len] = '\0';
    }
}

// duplicates string for manager keys where needed
static char *dupnull(const char *s){
    if (!s) return NULL;
    return strdup(s);
}

// creates the directory resultados if it doesn't exists
static void exist_result(void) {
    system ("mkdir -p resultados");
}

// helps writting header line to error file
static void wcsv_header(FILE *csv, FILE *ferror) {
    char header[2048];
    if(fgets(header, sizeof(header), csv)) {
        fprintf(ferror, "%s", header);
    }
}

void parse_airports(Dataset d, const char *data_path) {
    char filepath[512];
    snprintf(filepath, sizeof(filepath), "%s/airports.csv", data_path);

    FILE *f = fopen(filepath, "r");
    if (!f) { perror(filepath); return; }

    exist_result();
    FILE *ferror = fopen("resultados/airports_errors.csv", "w");
    if (!ferror) { perror("resultados/airports_errors.csv"); fclose(f); return; }

    wcsv_header(f, ferror);

    char line[2048];
    while (fgets(line, sizeof(line), f)) {
        char code[4] = "", name[101] = "", city[101] = "", country[51] = "";
        char lat_str[16] = "", long_str[16] = "", icao[5] = "", type[31] = "";

        int n = sscanf(line, "%3[^,],%100[^,],%100[^,],%50[^,],%16[^,],%16[^,],%4[^,],%30[^\n]",
                       code, name, city, country, lat_str, long_str, icao, type);
        if (n != 8) { 
            fprintf(ferror, "%s", line); 
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

        if (!validate_airport_code(code) ||
            !validate_latitude_longitude(lat_str, long_str) ||
            strlen(name) == 0) {
            fprintf(ferror, "%s", line);
            continue;
        }

        double latitude = strtod(lat_str, NULL);
        double longitude = strtod(long_str, NULL);

        Airport a = create_airport(code, name, city, country, latitude, longitude, icao, type);
        if (!a) {fprintf(stderr, "create_airport failed\n"); fprintf(ferror, "%s", line); continue; } 
    
        AirportsManager am = dataset_get_airports(d);
        airports_manager_add(am, a);
    }

    fclose(f);
    fclose(ferror);
}




void parse_aircrafts(Dataset d, const char *data_path) {
    char path[512];
    snprintf(path, sizeof(path), "%s/aircrafts.csv", data_path);

    FILE *f = fopen(path, "r");
    if (!f) { perror(path); return; }

    exist_result();
    FILE *ferror = fopen("resultados/aircrafts_errors.csv", "w");
    if (!ferror) { perror("resultados/aircrafts_errors.csv"); fclose(f); return; }

    wcsv_header(f, ferror);

    char line[2048];
    while (fgets(line, sizeof(line), f)) {
        char id[16] = "", manufacturer[51] = "", model[51] = "";
        char year_str[8] = "", capacity_str[8] = "", range_str[10] = "";

        int n = sscanf(line, "%15[^,],%50[^,],%50[^,],%7[^,],%7[^,],%9[^\n]",
                       id, manufacturer, model, year_str, capacity_str, range_str);
        if (n != 6) { fprintf(ferror, "%s", line); continue; }

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
            fprintf(ferror, "%s", line);
            continue;
        }

        Aircraft a = create_aircraft(id, manufacturer, model, year, capacity, range);
        if (!a) {fprintf(stderr, "create_aircraft failed\n"); fprintf(ferror, "%s", line); continue; }

        AircraftsManager am = dataset_get_aircrafts(d);
        aircrafts_manager_add(am, a);
    }

    fclose(f);
    fclose(ferror);
}

void parse_flights(Dataset d, const char *data_path) {
    char path[512];
    snprintf(path, sizeof(path), "%s/flights.csv", data_path);

    FILE *f = fopen(path, "r");
    if (!f) { perror(path); return; }

    exist_result();
    FILE *ferror = fopen("resultados/flights_errors.csv", "w");
    if (!ferror) { perror("resultados/flights_errors.csv"); fclose(f); return; }

    wcsv_header(f, ferror);

    char line[2048];
    while (fgets(line, sizeof(line), f)) {
        char flight_id[9] = "", departure[20] = "", actual_departure[20] = "", arrival[20] = "", actual_arrival[20] = "";
        char gate[4] = "", origin[5] = "", destination[5] = "", aircraft_id[16] = "", airline[41] = "", tracking_url[101] = "", status_str[16] = "";

        int n = sscanf(line, "%9[^,],%20[^,],%20[^,],%20[^,],%20[^,],%4[^,],%5[^,],%5[^,],%16[^,],%41[^,],%101[^,],%16[^\n]",
                       flight_id, departure, actual_departure, arrival, actual_arrival,
                       gate, origin, destination, aircraft_id, airline, tracking_url, status_str);

        if (n != 12) { fprintf(ferror, "%s", line); continue; }

        remove_quotes(flight_id); remove_spc(flight_id);
        remove_quotes(aircraft_id); remove_spc(aircraft_id);
        remove_quotes(status_str); remove_spc(status_str);
        remove_quotes(origin); remove_spc(origin);
        remove_quotes(destination); remove_spc(destination);

        flight_status status;
        if (strcmp(status_str, "OnTime") == 0) status = OnTime;
        else if (strcmp(status_str, "Delayed") == 0) status = Delayed;
        else if (strcmp(status_str, "Cancelled") == 0) status = Cancelled;
        else { fprintf(ferror, "%s", line); continue; }

        if (!validate_aircraft_id(aircraft_id, dataset_get_aircrafts(d))) {
            fprintf(ferror, "%s", line);
            continue;
        }
        
        Flight fv = create_flight(flight_id, departure, actual_departure, arrival,
                                  actual_arrival, gate, status, origin,
                                  destination, aircraft_id, airline, tracking_url);
        if (!fv) {fprintf(stderr, "create_flight failed\n"); fprintf(ferror, "%s", line); continue;}

        FlightsManager fm = dataset_get_flights(d);
        flights_manager_add(fm, fv);
    }

    fclose(f);
    fclose(ferror);
}

void parse_passengers(Dataset d, const char *data_path) {
    char path[512];
    snprintf(path, sizeof(path), "%s/passengers.csv", data_path);

    FILE *f = fopen(path, "r");
    if (!f) { perror(path); return; }

    exist_result();
    FILE *ferror = fopen("resultados/passengers_errors.csv", "w");
    if (!ferror) { perror("resultados/passengers_errors.csv"); fclose(f); return; }

    wcsv_header(f, ferror);

    char line[2048];
    while (fgets(line, sizeof(line), f)) {
        char document_id[11] = "", first_name[31] = "", last_name[31] = "", dob[12] = "";
        char nationality[21] = "", gender[11] = "", email[51] = "", phone[16] = "", address[51] = "", photo[1001] = "";

        int n = sscanf(line, "%10[^,],%30[^,],%30[^,],%11[^,],%20[^,],%10[^,],%50[^,],%15[^,],%50[^,],%1000[^\n]",
                       document_id, first_name, last_name, dob, nationality,
                       gender, email, phone, address, photo);

        if (n != 10) { fprintf(ferror, "%s", line); continue; }

        remove_quotes(document_id); remove_spc(document_id);
        remove_quotes(first_name); remove_spc(first_name);
        remove_quotes(last_name); remove_spc(last_name);
        remove_quotes(dob); remove_spc(dob);
        remove_quotes(nationality); remove_spc(nationality);
        remove_quotes(gender); remove_spc(gender);
        remove_quotes(email); remove_spc(email);
        remove_quotes(phone); remove_spc(phone);
        remove_quotes(address); remove_spc(address);
        remove_quotes(photo); remove_spc(photo);

        if (!validate_email(email) ||
            !validate_document_number(document_id) ||
            !validate_gender(gender) ||
            !validate_date(dob)) {
            fprintf(ferror, "%s", line);
            continue;
        }

        Passenger p = create_passenger(document_id, first_name, last_name, dob,
                                       nationality, gender, email, phone, address, photo);
        if (!p) { fprintf(stderr, "create_passenger failed"); fprintf(ferror, "%s", line); continue; }

        PassengersManager pm = dataset_get_passengers(d);
        passengers_manager_add(pm, p);
        }

    fclose(f);
    fclose(ferror);
}


void parse_reservations(Dataset d, const char *data_path) {
    char path[512];
    snprintf(path, sizeof(path), "%s/reservations.csv", data_path);

    FILE *f = fopen(path, "r");
    if (!f) { perror(path); return; }

    exist_result();
    FILE *ferror = fopen("resultados/reservations_errors.csv", "w");
    if (!ferror) { perror("resultados/reservations_errors.csv"); fclose(f); return; }

    wcsv_header(f, ferror);

    char line[2048];
    while (fgets(line, sizeof(line), f)) {
        char reservation_id[21] = "", flight_id[2][11] = "", document_number[16] = "";
        char qr_code[51] = "";
        int seat[2] = {0}, extra_luggage[2] = {0}, priority_boarding[2] = {0};
        double price[2] = {0.0};

        int n = sscanf(line, "%20[^,],%10[^,],%10[^,],%15[^,],%d,%d,%lf,%lf,%d,%d,%d,%d,%50[^\n]",
                       reservation_id, flight_id[0], flight_id[1], document_number,
                       &seat[0], &seat[1], &price[0], &price[1],
                       &extra_luggage[0], &extra_luggage[1],
                       &priority_boarding[0], &priority_boarding[1],
                       qr_code);

        if (n != 13) { fprintf(ferror, "%s", line); continue; }

        if (!passengers_manager_exists(dataset_get_passengers(d), document_number)) {
            fprintf(ferror, "%s", line);
            continue;
        }

        Reservation r = create_reservation(reservation_id, flight_id, document_number,
                                           seat, price, extra_luggage,
                                           priority_boarding, qr_code);
        if (!r) {fprintf(stderr, "create_reservation failed\n"); fprintf(ferror, "%s", line); continue;}
        
        ReservationsManager rm = dataset_get_reservations(d);
        reservations_manager_add(rm, r);
    }

    fclose(f);
    fclose(ferror);
}