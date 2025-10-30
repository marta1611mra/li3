#ifndef AIRCRAFTS_MANAGER_H
#define AIRCRAFTS_MANAGER_H
#include "aircrafts.h"
#include <glib.h>

typedef struct aircrafts_manager *AircraftsManager;

AircraftsManager create_aircrafts_manager();
void destroy_aircrafts_manager(AircraftsManager m);
void aircrafts_manager_add(AircraftsManager m, Aircraft a);
Aircraft aircrafts_manager_get(AircraftsManager m, const char *id);
int aircrafts_manager_count(AircraftsManager m);

#endif
