#ifndef FLIGHTS_H
#define FLIGHTS_H

typedef struct flight *Flight;

typedef enum {
    OnTime,
    Delayed,
    Cancelled
} flight_status;

Flight create_flight(const char *flight_id, const char *departure, const char *actual_departure, const char *arrival, const char *actual_arrival, const char *gate, flight_status status, const char *origin, const char *destination, const char *aircraft_id, const char *airline, const char *tracking_url);

void destroy_flight(Flight f);

const char *get_orig(Flight f);

const char *get_dest(Flight f);

flight_status get_stat(Flight f);

const char *get_flight_id(Flight f);

#endif