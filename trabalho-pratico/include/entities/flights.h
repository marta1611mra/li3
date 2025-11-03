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

const char *get_flight_orig(Flight f);

const char *get_flight_dest(Flight f);

flight_status get_flight_status(Flight f);

const char *get_flight_id(Flight f);

const char *get_flight_dep(Flight f);

const char *get_flight_actual_dep(Flight f);

const char *get_flight_arrival(Flight f);

const char *get_flight_actual_ar(Flight f);

const char *get_flight_gate(Flight f);

const char *get_flight_aircraft_id(Flight f);

const char *get_flight_airline(Flight f);

#endif