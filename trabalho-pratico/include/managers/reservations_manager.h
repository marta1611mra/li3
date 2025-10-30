#ifndef RESERVATIONS_MANAGER_H
#define RESERVATIONS_MANAGER_H
#include "reservations.h"
#include <glib.h>

typedef struct reservations_manager *ReservationsManager;

ReservationsManager create_reservations_manager();
void destroy_reservations_manager(ReservationsManager m);
void reservations_manager_add(ReservationsManager m, Reservation r);
Reservation reservations_manager_get(ReservationsManager m, const char *id);
int reservations_manager_count(ReservationsManager m);

#endif
