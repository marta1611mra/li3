#ifndef RESERVATIONS_MANAGER_H
#define RESERVATIONS_MANAGER_H
#include "reservations.h"
#include <glib.h>

/**
 * @brief Opaque pointer para a estrutura ReservationsManager.
 *
 * Gerencia todas as reservas do sistema.
 */
typedef struct reservations_manager *ReservationsManager;

/**
 * @brief Cria um novo ReservationsManager.
 *
 * Inicializa a estrutura que irá armazenar todas as reservas.
 *
 * @return Ponteiro para o novo ReservationsManager criado. NULL se falhar a alocação.
 */
ReservationsManager reservations_manager_create();

/**
 * @brief Liberta a memória associada ao ReservationsManager.
 *
 * @param m ReservationsManager a destruir. Ignorado se for NULL.
 */
void reservations_manager_destroy(ReservationsManager m);

/**
 * @brief Adiciona uma reserva ao ReservationsManager.
 *
 * @param m ReservationsManager.
 * @param r Reservation a adicionar.
 */
void reservations_manager_add(ReservationsManager m, Reservation r);

/**
 * @brief Obtém uma reserva pelo seu ID.
 *
 * @param m ReservationsManager.
 * @param id ID da reserva a procurar.
 * @return Ponteiro para a reserva encontrada ou NULL se não existir.
 */
Reservation reservations_manager_get(ReservationsManager m, const char *id);

/**
 * @brief Obtém o número total de reservas armazenadas.
 *
 * @param m ReservationsManager.
 * @return Número de reservas.
 */
int reservations_manager_count(ReservationsManager m);

#endif
