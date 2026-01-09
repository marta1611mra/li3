#ifndef FLIGHTS_MANAGER_H
#define FLIGHTS_MANAGER_H
#include "flights.h"
#include <glib.h>

/**
 * @brief Opaque pointer para a estrutura FlightsManager.
 *
 * Gerencia todos os voos do sistema.
 */
typedef struct flights_manager *FlightsManager;

/**
 * @brief Cria um novo FlightsManager.
 *
 * Inicializa a estrutura que irá armazenar todos os voos.
 *
 * @return Ponteiro para o novo FlightsManager criado. NULL se falhar a alocação.
 */
FlightsManager flights_manager_create();

/**
 * @brief Liberta a memória associada ao FlightsManager.
 *
 * @param m FlightsManager a destruir. Ignorado se for NULL.
 */
void flights_manager_destroy(FlightsManager m);

/**
 * @brief Adiciona um voo ao FlightsManager.
 *
 * @param m FlightsManager.
 * @param f Flight a adicionar.
 */
void flights_manager_add(FlightsManager m, Flight f);

/**
 * @brief Obtém um voo pelo seu ID.
 *
 * @param m FlightsManager.
 * @param id ID do voo a procurar.
 * @return Ponteiro para o voo encontrado ou NULL se não existir.
 */
Flight flights_manager_get(FlightsManager m, const char *id);

/**
 * @brief Obtém o código do aeroporto de destino do voo.
 *
 * @param f Flight.
 * @return String com o código do aeroporto de destino, ou NULL se não existir.
 */
const char *get_flight_destination(Flight f);

/**
 * @brief Obtém a data e hora de chegada do voo.
 *
 * @param f Flight.
 * @return String com a data e hora no formato "YYYY-MM-DD HH:MM", ou NULL se não existir.
 */
const char *get_flight_schedule_arrival_date(Flight f);


#endif
