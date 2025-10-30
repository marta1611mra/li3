#ifndef PASSENGERS_MANAGER_H
#define PASSENGERS_MANAGER_H
#include "passengers.h"
#include <glib.h>

typedef struct passengers_manager *PassengersManager;

PassengersManager create_passengers_manager();
void destroy_passengers_manager(PassengersManager m);
void passengers_manager_add(PassengersManager m, Passenger p);
Passenger passengers_manager_get(PassengersManager m, const char *id);
int passengers_manager_count(PassengersManager m);

#endif
