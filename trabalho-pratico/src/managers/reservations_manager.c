/**
 * @file reservations_manager.c
 * @brief Implementação das funções para gestão de reservas (ReservationsManager) usando GHashTable.
 *
 * O ReservationsManager armazena e gerencia múltiplas reservas, permitindo
 * adicionar, consultar e contar reservas.
 */

#include <string.h>
#include "reservations_manager.h"
#include "reservations.h"
#include <glib.h>
#include <stdlib.h>


/**
 * @struct reservations_manager
 * @brief Estrutura que representa um gestor de reservas.
 */
struct reservations_manager {
    GHashTable *reservations; /**< Tabela hash com chave = reservation_id e valor = ponteiro para Reservation. */
};

/**
 * @brief Cria um gestor de reservas.
 *
 * @return Ponteiro para o gestor criado ou NULL se falhar a alocação.
 */
ReservationsManager reservations_manager_create() {
    ReservationsManager m = malloc(sizeof(*m));
    if (!m) return NULL;

    m->reservations = g_hash_table_new_full(g_str_hash, g_str_equal, free, (GDestroyNotify)destroy_reservation);
    return m;
}

/**
 * @brief Destroi um gestor de reservas, libertando todos os recursos associados.
 *
 * @param m Ponteiro para o gestor a destruir.
 */
void reservations_manager_destroy(ReservationsManager m) {
    if (!m) return;
    g_hash_table_destroy(m->reservations);
    free(m);
}

/**
 * @brief Adiciona uma reserva ao gestor.
 *
 * @param m Ponteiro para o gestor.
 * @param r Ponteiro para a reserva a adicionar.
 */
void reservations_manager_add(ReservationsManager m, Reservation r) {
    g_hash_table_insert(m->reservations, strdup(get_reservation_id(r)), r);
}

/**
 * @brief Obtém uma reserva pelo seu ID.
 *
 * @param m Ponteiro para o gestor.
 * @param id ID da reserva.
 * @return Ponteiro para a reserva ou NULL se não existir.
 */
Reservation reservations_manager_get(ReservationsManager m, const char *id) {
    return g_hash_table_lookup(m->reservations, id);
}

/**
 * @brief Obtém o número de reservas no gestor.
 *
 * @param m Ponteiro para o gestor.
 * @return Número de reservas.
 */
int reservations_manager_count(ReservationsManager m) {
    return g_hash_table_size(m->reservations);
}
