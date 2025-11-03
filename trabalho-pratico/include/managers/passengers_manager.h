#ifndef PASSENGERS_MANAGER_H
#define PASSENGERS_MANAGER_H
#include "passengers.h"
#include <glib.h>
#include <stdbool.h>

typedef struct passengers_manager *PassengersManager;

PassengersManager passengers_manager_create();
void passengers_manager_destroy(PassengersManager m);
void passengers_manager_add(PassengersManager m, Passenger p);
Passenger passengers_manager_get(PassengersManager m, const char *id);
int passengers_manager_count(PassengersManager m);
bool passengers_manager_exists(PassengersManager m, const char *document_id);

#endif
