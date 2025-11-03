#ifndef LOGICAL_VALIDATION_H
#define LOGICAL_VALIDATION_H

#include <stdbool.h>
#include "entities/flights.h"
#include "entities/reservations.h"
#include "entities/passengers.h"
#include "managers/aircrafts_manager.h"

bool validate_destination(Flight f);
bool validate_arrival(Flight f);
bool validate_aircraft(const char *aircraft_id, AircraftsManager am);
bool validate_status(Flight f);
bool logical_validate_document_number(Reservation r, Passenger p[], int N);

#endif