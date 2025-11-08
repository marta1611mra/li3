#ifndef AIRCRAFTS_MANAGER_H
#define AIRCRAFTS_MANAGER_H
#include "aircrafts.h"
#include <glib.h>
#include <stdbool.h>

typedef struct aircrafts_manager *AircraftsManager;

AircraftsManager aircrafts_manager_create();
void aircrafts_manager_destroy(AircraftsManager m);
void aircrafts_manager_add(AircraftsManager m, Aircraft a);
Aircraft aircrafts_manager_get(AircraftsManager m, const char *id);
int aircrafts_manager_count(AircraftsManager m);
bool aircrafts_manager_exists(AircraftsManager m, const char *aircraft_id);
GHashTable *aircrafts_manager_get_table(AircraftsManager m);

#endif
