#include "flights.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct flight{
    
    char flight_id[8];
    char departure[19];
    char actual_departure[19];
    char arrival[19];
    char actual_arrival[19];
    char gate[3];
    flight_status status;
    char origin[4];
    char destination[4];
    char aircraft_id[15];
    char airline[40];
    char tracking_url[100];
};


Flight create_flight(const char *flight_id, const char *departure, const char *actual_departure, const char *arrival, const char *actual_arrival, const char *gate, flight_status status, const char *origin, const char *destination, const char *aircraft_id, const char *airline, const char *tracking_url) {
    Flight f = malloc(sizeof(struct flight));
    if (!f) return NULL;

    strcpy(f->flight_id, flight_id);
    strcpy(f->departure, departure);
    strcpy(f->actual_departure, actual_departure);
    strcpy(f->arrival, arrival);
    strcpy(f->actual_arrival, actual_arrival);
    strcpy(f->gate, gate);
    f->status = status;
    strcpy(f->origin, origin);
    strcpy(f->destination, destination);
    strcpy(f->aircraft_id, aircraft_id);
    strcpy(f->airline, airline);
    strcpy(f->tracking_url, tracking_url);

    return f;
}

void destroy_flight(Flight f) {
    if (f) free(f);
}

const char *get_orig(Flight f) {
    return f->origin; 
}

const char *get_dest(Flight f) {
    return f->destination; 
}

flight_status get_stat(Flight f) {
    return f->status; 

}

const char *get_flight_id(Flight f) { 
    return f->flight_id; 
}

