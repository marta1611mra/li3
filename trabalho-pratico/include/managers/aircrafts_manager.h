#ifndef AIRCRAFTS_MANAGER_H
#define AIRCRAFTS_MANAGER_H
#include "aircrafts.h"
#include <glib.h>
#include <stdbool.h>

/**
 * @brief Opaque pointer para a estrutura AircraftsManager.
 *
 * Gerencia todas as aeronaves do sistema.
 */
typedef struct aircrafts_manager *AircraftsManager;

/**
 * @brief Cria um novo AircraftsManager.
 *
 * Inicializa a estrutura que irá armazenar todas as aeronaves.
 *
 * @return Ponteiro para o novo AircraftsManager criado. NULL se falhar a alocação.
 */
AircraftsManager aircrafts_manager_create();

/**
 * @brief Liberta a memória associada ao AircraftsManager.
 *
 * @param m AircraftsManager a destruir. Ignorado se for NULL.
 */
void aircrafts_manager_destroy(AircraftsManager m);

/**
 * @brief Adiciona uma aeronave ao AircraftsManager.
 *
 * @param m AircraftsManager.
 * @param a Aircraft a adicionar.
 */
void aircrafts_manager_add(AircraftsManager m, Aircraft a);

/**
 * @brief Obtém uma aeronave pelo seu ID.
 *
 * @param m AircraftsManager.
 * @param id ID da aeronave a procurar.
 * @return Ponteiro para a aeronave encontrada ou NULL se não existir.
 */
Aircraft aircrafts_manager_get(AircraftsManager m, const char *id);

/**
 * @brief Obtém o número total de aeronaves armazenadas.
 *
 * @param m AircraftsManager.
 * @return Número de aeronaves.
 */
int aircrafts_manager_count(AircraftsManager m);

/**
 * @brief Verifica se uma aeronave existe no manager.
 *
 * @param m AircraftsManager.
 * @param aircraft_id ID da aeronave.
 * @return true se a aeronave existir, false caso contrário.
 */
bool aircrafts_manager_exists(AircraftsManager m, const char *aircraft_id);

/**
 * @brief Obtém a tabela hash interna do manager.
 *
 * Útil para iterações ou operações avançadas.
 *
 * @param m AircraftsManager.
 * @return Ponteiro para a GHashTable que contém todas as aeronaves.
 */
GHashTable *aircrafts_manager_get_table(AircraftsManager m);

#endif
