#include "parser.h"
#include "syntactic_validation.h"
#include "logical_validation.h"
#include "flights.c"
#include "reservations.c"
#include "passengers.c"
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

    system("mkdir -p resultados"); // Garantees that the directory exists
    FILE *ferr = fopen("resultados/airports_errors.csv", "w");
    if (!ferr) {
        perror("ERROR: COUNDN'T CREATE airports_errors.csv");
        fclose(f); // closes the opened file (airports.csv)
        return;
    }

    char line[512];
    char header[512]; // stores the first line of the csv file
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

/*
void parse_aircrafts(Dataset d, const char *dataset_path) {
    char path[256];
    snprintf(path, sizeof(path), "%s/aircrafts.csv", dataset_path);

    FILE *f = fopen(path, "r");
    if(!f) {
        perror("ERROR: COULDN'T OPEN aircrafts.csv");
        return;
    }

    FILE *ferror = fopen("resultados/aircrafts_error.csv", "w");
    if(!ferror) {
        perror("ERROR: COULDN'T CREATE aircrafts_error.csv");
        fclose(f);
        return;
    }
    
    char line[512];
    char header[512];
    fgets(header, sizeof(header), f);
    fprintf(ferror, "%s", header);

    while (fgets(line, sizeof(line), f)) {
        char id[15], manufacturer[50], model[50];
        char year_str[5], capacity_str[5], range_str[5];

        int n = sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^,],%[^\n]",
                       id, manufacturer, model, year_str, capacity_str, range_str);
        
        if (n != 6) {
            fprintf(ferror, "%s", line);
            continue;
        }

        remove_quotes(id); remove_spc(id);
        remove_quotes(manufacturer); remove_spc(manufacturer);
        remove_quotes(model); remove_spc(model);
        remove_quotes(year_str); remove_spc(year_str);
        remove_quotes(capacity_str); remove_spc(capacity_str);
        remove_quotes(range_str); remove_spc(range_str);

        if () {
            continue;
        }

        int year = atoi(year_str);
        int capacity = atoi(capacity_str);
        int range = atoi(range_str);

        Aircraft ac = create_aircraft(id, manufacturer, model, year, capacity, range);
        d->aircrafts = g_list_append(d->aircrafts,ac);
    }

    fclose(f);
    fclose(ferror);
}
*/

void parse_flights(Dataset d, const char *dataset_path) {
    char path[256];
    snprintf(path, sizeof(path), "%s/flights.csv", dataset_path);

    FILE *f = fopen(path, "r");
    if(!f) {
        perror("ERROR: COULDN'T OPEN flights.csv");
        return;
    }

    FILE *ferror = fopen("resultados/flights_error.csv", "w");
    if(!ferror) {
        perror("ERROR: COULDN'T CREATE flights_error.csv");
        fclose(f);
        return;
    }

    char line[512];
    char header[512];
    fgets(header, sizeof(header), f);
    fprintf(ferror, "%s", header);

    while (fgets(line, sizeof(line), f)) {
        char flight_id[8], departure[19], actual_departure[19], arrival[19], actual_arrival[19], gate[3], origin[4], destination[4], aircraft_id[15], airline[40], tracking_url[100];
        char status_str[15];

        int n = sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^\n]",
                       flight_id, departure, actual_departure, arrival, actual_arrival, gate, status_str, origin, destination, aircraft_id, airline, tracking_url);
        
        if (n != 12) {
            fprintf(ferror, "%s", line);
            continue;
        }

        remove_quotes(flight_id); remove_spc(flight_id);
        remove_quotes(departure); remove_spc(departure);
        remove_quotes(actual_departure); remove_spc(actual_departure);
        remove_quotes(arrival); remove_spc(arrival);
        remove_quotes(actual_arrival); remove_spc(actual_arrival);
        remove_quotes(gate); remove_spc(gate);
        remove_quotes(status_str); remove_spc(status_str);
        remove_quotes(origin); remove_spc(origin);
        remove_quotes(destination); remove_spc(destination);
        remove_quotes(aircraft_id); remove_spc(aircraft_id);
        remove_quotes(airline); remove_spc(airline);
        remove_quotes(tracking_url); remove_spc(tracking_url);

        //Converts status
        flight_status status;
        if (strcmp(status_str, "OnTime") == 0) status = OnTime;
        else if (strcmp(status_str, "Delayed") == 0) status = Delayed;
        else if (strcmp(status_str, "Cancelled") == 0) status = Cancelled;
        else {
            fprintf(ferror, "%s", line);
            continue;
        }

        //Create temporary struct Flight for validations
        struct flight ftmp;
        strcpy(ftmp.flight_id, flight_id);
        strcpy(ftmp.departure, departure);
        strcpy(ftmp.actual_departure, actual_departure);
        strcpy(ftmp.arrival, arrival);
        strcpy(ftmp.actual_arrival, actual_arrival);
        strcpy(ftmp.gate, gate);
        ftmp.status = status;
        strcpy(ftmp.origin, origin);
        strcpy(ftmp.destination, destination);
        strcpy(ftmp.aircraft_id, aircraft_id);
        strcpy(ftmp.airline, airline);
        strcpy(ftmp.tracking_url, tracking_url);

        // Validations
        if (!validate_destination(ftmp) ||
            !validate_arrival(ftmp) ||
            !validate_aircraft(ftmp, d->aircrafts, g_list_length(d->aircrafts)) ||
            !validate_status(ftmp)) {
                fprintf (ferror, "%s", line);
                continue;
            }
        
        Flight fv = create_flight(flight_id, departure, actual_departure, arrival, actual_arrival, gate, status, origin, destination, aircraft_id, airline, tracking_url);

        if (!fv) {
            fprintf(stderr, "ERRO: MEMORY ALLOCATION FAILED FOR FLIGHT %s\n", flight_id);
            continue;
        }

        d->flights = g_list_append(d->flights, fv);
    }
    fclose(f);
    fclose(ferror);
}

void parse_passengers(Dataset d, const char *dataset_path) {
    char path[256];
    snprintf(path, sizeof(path), "%s/passengers.csv", dataset_path);

    FILE *f = fopen(path, "r");
    if (!f) {
        perror("ERROR: COULDN'T OPEN passengers.csv");
        return;
    }

    system("mkdir -p resultados"); // Garantees that the directory exists
    FILE *ferror = fopen("resultados/passengers_errors.csv", "w");
    if (!ferror) {
        perror("ERROR: COUNDN'T CREATE passengers_errors.csv");
        fclose(f); // closes the opened file (airports.csv)
        return;
    }

    char line[512];
    char header[512]; // stores the first line of the csv file
    fgets(header, sizeof(header), f);
    fprintf(ferror, "%s", header); //copies header to the error file

    while (fgets(line, sizeof(line), f)) {
        char document_id[10], first_name [30], last_name[30], dob[11];
        char nationality[20], gender[10], email[50], phone[15], address[50], photo[1000];

        int n = sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^\n]",
                       document_id, first_name, last_name, dob, nationality, gender, email, phone, address, photo);

        if (n != 10) {
            fprintf(ferror, "%s", line);
            continue;
        }

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

        // validate data
        if (!validate_email(email) ||
            !validate_document_number(document_id) ||
            !validate_gender(gender) ||
            !validate_date(dob)) {
            fprintf(ferror, "%s", line);
            continue;
        }

        // Create entitie and add to the list
        Passenger p = create_passenger(document_id, first_name, last_name, dob, nationality, gender, email, phone, address, photo);
        d->passengers = g_list_append(d->passengers, p);
    }

    fclose(f);
    fclose(ferror);
}

void parse_reservations(Dataset d, const char *dataset_path) {
    char path[256];
    snprintf(path, sizeof(path), "%s/reservations.csv", dataset_path);

    FILE *f = fopen(path, "r");
    if (!f) {
        perror("ERROR: COULDN'T OPEN reservations.csv");
        return;
    }

    system("mkdir -p resultados"); // Garantees that the directory exists
    
    FILE *ferror = fopen("resultados/reservations_errors.csv", "w");
    if (!ferror) {
        perror("ERROR: COUNDN'T CREATE reservations_errors.csv");
        fclose(f); // closes the opened file (airports.csv)
        return;
    }

    char line[512];
    char header[512]; // stores the first line of the csv file
    fgets(header, sizeof(header), f);
    fprintf(ferror, "%s", header); //copies header to the error file

    while (fgets(line, sizeof(line), f)) {
        char reservation_id[10]; 
        char flight_id_str[2][100];
        char document_number[15];
        char seat_str[2][10], price_str[2][30], extra_luggage_str[2][30], priority_boarding_str[2][10];
        char qr_code[30];

        int n = sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^\n]",
                       reservation_id, flight_id_str[0], flight_id_str[1], document_number, seat_str[0], seat_str[1], price_str[0], price_str[1], extra_luggage_str[0], extra_luggage_str[1], priority_boarding_str[0], priority_boarding_str[1], qr_code);

        if (n != 13) {
            fprintf(ferror, "%s", line);
            continue;
        }

        for (int i = 0; i<2; i++) {
            remove_quotes(flight_id_str[i]); remove_spc(flight_id_str[i]);
            remove_quotes(seat_str[i]); remove_spc(seat_str[i]);
            remove_quotes(price_str[i]); remove_spc(price_str[i]);
            remove_quotes(extra_luggage_str[i]); remove_spc(extra_luggage_str[i]);
            remove_quotes(priority_boarding_str[i]); remove_spc(priority_boarding_str[i]);
        }
        remove_quotes(reservation_id); remove_spc(reservation_id);
        remove_quotes(document_number); remove_spc(document_number);
        remove_quotes(qr_code); remove_spc(qr_code);

        int seat[2], extra_luggage[2], priority_boarding[2];
        double price[2];
        for (int i = 0; i < 2; i++) {
            seat[i] = atoi(seat_str[i]);
            price[i] = strtod(price_str[i], NULL);
            extra_luggage[i] = atoi(extra_luggage_str[i]);
            priority_boarding[i] = atoi(priority_boarding_str[i]);
        }

        struct reservation rtmp;
        strcpy(rtmp.reservation_id, reservation_id);
        strcpy(rtmp.flight_id[0], flight_id_str[0]);
        strcpy(rtmp.flight_id[1], flight_id_str[1]);
        strcpy(rtmp.document_number, document_number);
        for (int i = 0; i < 2; i++) {
            rtmp.seat[i] = seat[i];
            rtmp.price[i] = price[i];
            rtmp.extra_luggage[i] = extra_luggage[i];
            rtmp.priority_boarding[i] = priority_boarding[i];
        }
        strcpy(rtmp.qr_code, qr_code);

        // validate data
        if (!validate_flight_id(rtmp.flight_id[0]) ||
            !validate_flight_id(rtmp.flight_id[1]) ||
            !validate_reservation_id(rtmp.reservation_id) ||
            !validate_document_number(rtmp.document_number)) {
            fprintf(ferror, "%s", line);
            continue;
        }

        // Validates the existence of passenger in the dataset
        bool passenger_exists = false;
        for (GList *l = d->passengers; l != NULL; l = l->next) {
            struct passenger *p = (struct passenger *)l->data;
            if (strcmp(rtmp.document_number, p->document_id) == 0) {
                passenger_exists = true;
                break;
            }
        }
        if (!passenger_exists) {
            fprintf(ferror, "%s", line);
            continue;
        }

        // Creates entity and adds to the list
        Reservation r = create_reservation(reservation_id, rtmp.flight_id, document_number, rtmp.seat, rtmp.price, rtmp.extra_luggage, rtmp.priority_boarding, qr_code);
        if (!r) {
            fprintf(stderr, "ERROR: MEMORY ALLOCATION FAILED FOR RESERVATION %s\n", reservation_id);
            continue;
        }
        d->reservations = g_list_append(d->reservations, r);
    }

    fclose(f);
    fclose(ferror);
}