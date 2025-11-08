#ifndef LOGICAL_VALIDATION_H
#define LOGICAL_VALIDATION_H

#include <stdbool.h>
#include "entities/flights.h"
#include "entities/reservations.h"
#include "entities/passengers.h"
#include "managers/aircrafts_manager.h"

bool validate_destination(const char *orig, const char *dest);
bool validate_arrival(const char *departure,
                      const char *actual_departure,
                      const char *arrival,
                      const char *actual_arrival,
                      flight_status status) ;
bool validate_aircraft(const char *aircraft_id, AircraftsManager am);
bool validate_status(flight_status status,
                     const char *actual_departure,
                     const char *actual_arrival);
bool logical_validate_document_number(Reservation r, Passenger p[], int N);

#endif