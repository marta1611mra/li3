#ifndef PASSENGERS_MANAGER_H
#define PASSENGERS_MANAGER_H

#include "passengers.h"
#include <glib.h>
#include <stdbool.h>

/**
 * @brief Opaque pointer para a estrutura PassengersManager.
 *
 * Gerencia todos os passageiros do sistema.
 */
typedef struct passengers_manager *PassengersManager;

/**
 * @brief Cria um novo PassengersManager.
 *
 * Inicializa a estrutura que irá armazenar todos os passageiros.
 *
 * @return Ponteiro para o novo PassengersManager criado. NULL se falhar a alocação.
 */
PassengersManager passengers_manager_create();

/**
 * @brief Liberta a memória associada ao PassengersManager.
 *
 * @param m PassengersManager a destruir. Ignorado se for NULL.
 */
void passengers_manager_destroy(PassengersManager m);

/**
 * @brief Adiciona um passageiro ao PassengersManager.
 *
 * @param m PassengersManager.
 * @param p Passenger a adicionar.
 */
void passengers_manager_add(PassengersManager m, Passenger p);

/**
 * @brief Obtém um passageiro pelo seu número de documento.
 *
 * @param m PassengersManager.
 * @param id Número de documento do passageiro.
 * @return Ponteiro para o passageiro encontrado ou NULL se não existir.
 */
Passenger passengers_manager_get(PassengersManager m, const char *id);

/**
 * @brief Verifica se um passageiro existe no manager.
 *
 * @param m PassengersManager.
 * @param document_id Número de documento do passageiro.
 * @return true se o passageiro existir, false caso contrário.
 */
bool passengers_manager_exists(PassengersManager m, const char *document_id);

#endif
