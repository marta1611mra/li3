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
 * @brief Obtém o número total de voos armazenados.
 *
 * @param m FlightsManager.
 * @return Número de voos.
 */
int flights_manager_count(FlightsManager m);

/**
 * @brief Obtém a tabela hash interna do manager.
 *
 * Útil para iterações ou operações avançadas.
 *
 * @param m FlightsManager.
 * @return Ponteiro para a GHashTable que contém todos os voos.
 */
GHashTable *flights_manager_get_table(FlightsManager m);

#endif
