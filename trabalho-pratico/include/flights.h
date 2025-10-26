#ifndef FLIGHTS_H
#define FLIGHTS_H

typedef struct flight *Flight;

Flight create_flight(const char *flight_id, const char *departure, const char *actual_departure, const char *arrival, const char *actual_arrival, const char *gate, flight_status status, const char *origin, const char *destination, const char *aircraft_id, const char *airline, const char *tracking_url);

void destroy_flight(Flight f);

 #endif