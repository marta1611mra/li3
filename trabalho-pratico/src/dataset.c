/**
 * @file dataset.c
 * @brief Implementação do Dataset: agregador de todos os gestores do sistema.
 *
 * O Dataset funciona como a estrutura principal que contém os cinco gestores:
 * - AirportsManager
 * - AircraftsManager
 * - FlightsManager
 * - PassengersManager
 * - ReservationsManager
 *
 * Cada gestor é responsável por armazenar e manipular os respetivos dados.
 */

#include "dataset.h"
#include <stdlib.h>

/**
 * @struct dataset
 * @brief Estrutura interna que agrega todos os gestores do sistema.
 *
 * Cada campo corresponde a um gestor específico.
 */
struct dataset {
    AirportsManager airports;       /** Gestor de aeroportos */
    AircraftsManager aircrafts;     /** Gestor de aeronaves */
    FlightsManager flights;         /** Gestor de voos */
    PassengersManager passengers;   /** Gestor de passageiros */
    ReservationsManager reservations; /** Gestor de reservas */
};

/**
 * @brief Cria e inicializa um novo Dataset.
 *
 * Esta função cria cada gestor individualmente e retorna um ponteiro
 * para a estrutura Dataset. Se alguma alocação falhar, retorna NULL.
 *
 * @return Ponteiro para Dataset inicializado, ou NULL se houver falha.
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
 * @brief Liberta toda a memória associada ao Dataset.
 *
 * Esta função destrói todos os gestores contidos no Dataset
 * e, por fim, liberta a própria estrutura.
 *
 * @param d Dataset a destruir. Ignorado se NULL.
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
 * @brief Obtém o gestor de aeroportos do Dataset.
 *
 * @param d Dataset
 * @return AirportsManager armazenado no Dataset
 */
AirportsManager dataset_get_airports(Dataset d) { 
    return d->airports; 
}

/**
 * @brief Obtém o gestor de aeronaves do Dataset.
 *
 * @param d Dataset
 * @return AircraftsManager armazenado no Dataset
 */
AircraftsManager dataset_get_aircrafts(Dataset d) { 
    return d->aircrafts; 
}

/**
 * @brief Obtém o gestor de voos do Dataset.
 *
 * @param d Dataset
 * @return FlightsManager armazenado no Dataset
 */
FlightsManager dataset_get_flights(Dataset d) { 
    return d->flights; 
}

/**
 * @brief Obtém o gestor de passageiros do Dataset.
 *
 * @param d Dataset
 * @return PassengersManager armazenado no Dataset
 */
PassengersManager dataset_get_passengers(Dataset d) { 
    return d->passengers; 
}

/**
 * @brief Obtém o gestor de reservas do Dataset.
 *
 * @param d Dataset
 * @return ReservationsManager armazenado no Dataset
 */
ReservationsManager dataset_get_reservations(Dataset d) { 
    return d->reservations; 
}
