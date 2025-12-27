#ifndef DATASET_H
#define DATASET_H

#include "managers/airports_manager.h"
#include "managers/aircrafts_manager.h"
#include "managers/flights_manager.h"
#include "managers/passengers_manager.h"
#include "managers/reservations_manager.h"

/**
 * @brief Opaque pointer para a estrutura Dataset.
 *
 * Representa o conjunto de dados completo do sistema,
 * contendo todos os managers (airports, aircrafts, flights, passengers e reservations).
 */
typedef struct dataset *Dataset;

/**
 * @brief Cria um novo Dataset.
 *
 * Inicializa todos os managers internos e retorna um ponteiro para o Dataset.
 *
 * @return Ponteiro para o Dataset criado. NULL se falhar a alocação.
 */
Dataset dataset_create(void);

GHashTable *dataset_get_q6_index(Dataset d); 
GHashTable *dataset_get_q2_index(Dataset d);

void dataset_update_q2(Dataset d, const char *aircraft_id, const char *manufacturer);

void dataset_build_q6_index(Dataset d);

void dataset_update_q6(Dataset d, const char *nationality, const char *airport);
/**
 * @brief Liberta a memória associada a um Dataset.
 *
 * Liberta todos os managers internos e a própria estrutura Dataset.
 *
 * @param d Dataset a destruir. Ignorado se for NULL.
 */
void dataset_destroy(Dataset d);

/**
 * @brief Obtém o manager de aeroportos do Dataset.
 *
 * @param d Dataset.
 * @return AirportsManager associado ao Dataset.
 */
AirportsManager dataset_get_airports(Dataset d);

/**
 * @brief Obtém o manager de aeronaves do Dataset.
 *
 * @param d Dataset.
 * @return AircraftsManager associado ao Dataset.
 */
AircraftsManager dataset_get_aircrafts(Dataset d);

/**
 * @brief Obtém o manager de voos do Dataset.
 *
 * @param d Dataset.
 * @return FlightsManager associado ao Dataset.
 */
FlightsManager dataset_get_flights(Dataset d);

/**
 * @brief Obtém o manager de passageiros do Dataset.
 *
 * @param d Dataset.
 * @return PassengersManager associado ao Dataset.
 */
PassengersManager dataset_get_passengers(Dataset d);

/**
 * @brief Obtém o manager de reservas do Dataset.
 *
 * @param d Dataset.
 * @return ReservationsManager associado ao Dataset.
 */
ReservationsManager dataset_get_reservations(Dataset d);

#endif
