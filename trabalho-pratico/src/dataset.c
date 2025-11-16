#include "dataset.h"
#include <stdlib.h>

/**
 * Estrutura interna que agrega todos os gestores necessários
 * ao funcionamento do sistema: aeroportos, aeronaves, voos,
 * passageiros e reservas.
 */

struct dataset {
    AirportsManager airports;
    AircraftsManager aircrafts;
    FlightsManager flights;
    PassengersManager passengers;
    ReservationsManager reservations;
};

/**
 * Cria e inicializa um novo Dataset contendo todos os gestores do sistema.
 *
 * Cada gestor é criado individualmente através das respetivas funções
 * de criação.
 *
 * @return Um ponteiro para o Dataset recém-criado,
 *         ou NULL caso a alocação de memória falhe.
 */

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

/**
 * Liberta toda a memória associada a um Dataset.
 *
 * Esta função destrói cada gestor individualmente e,
 * por fim, liberta a própria estrutura Dataset.
 *
 * @param d Dataset a destruir. Ignorado se for NULL.
 */

void dataset_destroy(Dataset d) {
    if (!d) return;
    airports_manager_destroy(d->airports);
    aircrafts_manager_destroy(d->aircrafts);
    flights_manager_destroy(d->flights);
    passengers_manager_destroy(d->passengers);
    reservations_manager_destroy(d->reservations);
    free(d);
}

/**
 * Obtém o gestor de aeroportos associado ao Dataset.
 * @param d Dataset.
 * @return AirportsManager armazenado no Dataset.
 */

AirportsManager dataset_get_airports(Dataset d) { 
    return d->airports; 
}

/**
 * Obtém o gestor de aeronaves associado ao Dataset.
 * @param d Dataset.
 * @return AircraftsManager armazenado no Dataset.
 */

AircraftsManager dataset_get_aircrafts(Dataset d) { 
    return d->aircrafts; 
}

/**
 * Obtém o gestor de voos associado ao Dataset.
 * @param d Dataset.
 * @return FlightsManager armazenado no Dataset.
 */

FlightsManager dataset_get_flights(Dataset d) { 
    return d->flights; 
}

/**
 * Obtém o gestor de passageiros associado ao Dataset.
 * @param d Dataset.
 * @return PassengersManager armazenado no Dataset.
 */

PassengersManager dataset_get_passengers(Dataset d) { 
    return d->passengers; 
}

/**
 * Obtém o gestor de reservas associado ao Dataset.
 * @param d Dataset.
 * @return ReservationsManager armazenado no Dataset.
 */
ReservationsManager dataset_get_reservations(Dataset d) { 
    return d->reservations; 
}
