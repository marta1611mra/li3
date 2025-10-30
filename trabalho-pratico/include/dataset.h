#ifndef DATASET_H
#define DATASET_H

#include "managers/airports_manager.h"
#include "managers/aircrafts_manager.h"
#include "managers/flights_manager.h"
#include "managers/passengers_manager.h"
#include "managers/reservations_manager.h"


typedef struct dataset *Dataset;


Dataset create_dataset();

void destroy_dataset(Dataset d);

AirportsManager dataset_get_airports(Dataset d);

AircraftsManager dataset_get_aircrafts(Dataset d);

FlightsManager dataset_get_flights(Dataset d);

PassengersManager dataset_get_passengers(Dataset d);

ReservationsManager dataset_get_reservations(Dataset d);

#endif
