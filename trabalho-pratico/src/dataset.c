#include "dataset.h"
#include <stdlib.h>

struct dataset {
    AirportsManager airports;
    AircraftsManager aircrafts;
    FlightsManager flights;
    PassengersManager passengers;
    ReservationsManager reservations;
};

Dataset create_dataset() {
    Dataset d = malloc(sizeof(*d));
    if (!d) return NULL;

    d->airports = create_airports_manager();
    d->aircrafts = create_aircrafts_manager();
    d->flights = create_flights_manager();
    d->passengers = create_passengers_manager();
    d->reservations = create_reservations_manager();

    return d;
}

void destroy_dataset(Dataset d) {
    if (!d) return;

    destroy_airports_manager(d->airports);
    destroy_aircrafts_manager(d->aircrafts);
    destroy_flights_manager(d->flights);
    destroy_passengers_manager(d->passengers);
    destroy_reservations_manager(d->reservations);

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
