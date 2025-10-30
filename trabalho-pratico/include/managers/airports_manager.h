#ifndef AIRPORTS_MANAGER_H
#define AIRPORTS_MANAGER_H
#include "airports.h"
#include <glib.h>

typedef struct airports_manager *AirportsManager;

AirportsManager create_airports_manager();
void destroy_airports_manager(AirportsManager m);
void airports_manager_add(AirportsManager m, Airport a);
Airport airports_manager_get(AirportsManager m, const char *code);
int airports_manager_count(AirportsManager m);

#endif
