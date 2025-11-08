#ifndef FLIGHTS_MANAGER_H
#define FLIGHTS_MANAGER_H
#include "flights.h"
#include <glib.h>

typedef struct flights_manager *FlightsManager;

FlightsManager flights_manager_create();
void flights_manager_destroy(FlightsManager m);
void flights_manager_add(FlightsManager m, Flight f);
Flight flights_manager_get(FlightsManager m, const char *id);
int flights_manager_count(FlightsManager m);
GHashTable *flights_manager_get_table(FlightsManager m);

#endif
