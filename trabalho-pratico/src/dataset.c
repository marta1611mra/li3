#include "dataset.h"
#include <stdlib.h>

struct dataset {
    AirportsManager airports;       /** Gestor de aeroportos */
    AircraftsManager aircrafts;     /** Gestor de aeronaves */
    FlightsManager flights;         /** Gestor de voos */
    PassengersManager passengers;   /** Gestor de passageiros */
    ReservationsManager reservations; /** Gestor de reservas */
};

// Cria e inicializa um novo Dataset.
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

// Liberta toda a memória associada ao Dataset.
void dataset_destroy(Dataset d) {
    if (!d) return;

    airports_manager_destroy(d->airports);
    aircrafts_manager_destroy(d->aircrafts);
    flights_manager_destroy(d->flights);
    passengers_manager_destroy(d->passengers);
    reservations_manager_destroy(d->reservations);
    free(d);
}

// Obtém o gestor de aeroportos do Dataset.
AirportsManager dataset_get_airports(Dataset d) { 
    return d->airports; 
}

// Obtém o gestor de aeronaves do Dataset.
AircraftsManager dataset_get_aircrafts(Dataset d) { 
    return d->aircrafts; 
}

// Obtém o gestor de voos do Dataset.
FlightsManager dataset_get_flights(Dataset d) { 
    return d->flights; 
}

// @brief Obtém o gestor de passageiros do Dataset.
PassengersManager dataset_get_passengers(Dataset d) { 
    return d->passengers; 
}

// Obtém o gestor de reservas do Dataset.
ReservationsManager dataset_get_reservations(Dataset d) { 
    return d->reservations; 
}
