#ifndef FLIGHTS_MANAGER_H
#define FLIGHTS_MANAGER_H
#include "flights.h"
#include <glib.h>

typedef struct flights_manager *FlightsManager;

FlightsManager create_flights_manager();
void destroy_flights_manager(FlightsManager m);
void flights_manager_add(FlightsManager m, Flight f);
Flight flights_manager_get(FlightsManager m, const char *id);
int flights_manager_count(FlightsManager m);

#endif
