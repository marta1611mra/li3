#include "dataset.h"
#include <stdlib.h>

struct dataset {
    AirportsManager airports;
    AircraftsManager aircrafts;
    FlightsManager flights;
    PassengersManager passengers;
    ReservationsManager reservations;
};

Dataset dataset_create(void) {
    Dataset d = malloc(sizeof(struct dataset));
    if (!d) return NULL;

    d->airports = airports_manager_create();
    d->aircrafts = aircrafts_manager_create();
    d->flights = flights_manager_create();
    d->passengers = passengers_manager_create();
    d->reservations = reservations_manager_create();

    return d;
}

void dataset_destroy(Dataset d) {
    if (!d) return;
    airports_manager_destroy(d->airports);
    aircrafts_manager_destroy(d->aircrafts);
    flights_manager_destroy(d->flights);
    passengers_manager_destroy(d->passengers);
    reservations_manager_destroy(d->reservations);
    free(d);
}


AirportsManager dataset_get_airports(Dataset d) { 
    return d->airports; 
}

AircraftsManager dataset_get_aircrafts(Dataset d) { 
    return d->aircrafts; 
}

FlightsManager dataset_get_flights(Dataset d) { 
    return d->flights; 
}
PassengersManager dataset_get_passengers(Dataset d) { 
    return d->passengers; 
}

ReservationsManager dataset_get_reservations(Dataset d) { 
    return d->reservations; 
}
