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

#endif