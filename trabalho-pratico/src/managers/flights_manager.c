/**
 * @file flights_manager.c
 * @brief Implementação das funções para gestão de voos (FlightsManager) usando GHashTable.
 *
 * O FlightsManager armazena e gerencia múltiplos voos, permitindo
 * adicionar, consultar, contar e aceder à tabela interna de voos.
 */

#include <string.h>
#include "flights_manager.h"
#include "flights.h"
#include <glib.h>
#include <stdlib.h>

/**
 * @struct flights_manager
 * @brief Estrutura que representa um gestor de voos.
 */
struct flights_manager {
    GHashTable *flights; /**< Tabela hash com chave = ID do voo e valor = ponteiro para Flight. */
};

/**
 * @brief Cria um gestor de voos.
 *
 * @return Ponteiro para o gestor criado ou NULL se falhar a alocação.
 */
FlightsManager flights_manager_create() {
    FlightsManager m = malloc(sizeof(*m));
    if (!m) return NULL;

    m->flights = g_hash_table_new_full(g_str_hash, g_str_equal, free, (GDestroyNotify)destroy_flight);
    return m;
}

/**
 * @brief Destroi um gestor de voos, libertando todos os recursos associados.
 *
 * @param m Ponteiro para o gestor a destruir.
 */
void flights_manager_destroy(FlightsManager m) {
    if (!m) return;
    g_hash_table_destroy(m->flights);
    free(m);
}

/**
 * @brief Adiciona um voo ao gestor.
 *
 * @param m Ponteiro para o gestor.
 * @param f Ponteiro para o voo a adicionar.
 */
void flights_manager_add(FlightsManager m, Flight f) {
    g_hash_table_insert(m->flights, strdup(get_flight_id(f)), f);
}

/**
 * @brief Obtém um voo pelo seu ID.
 *
 * @param m Ponteiro para o gestor.
 * @param id ID do voo.
 * @return Ponteiro para o voo ou NULL se não existir.
 */
Flight flights_manager_get(FlightsManager m, const char *id) {
    return g_hash_table_lookup(m->flights, id);
}

/**
 * @brief Obtém o número de voos no gestor.
 *
 * @param m Ponteiro para o gestor.
 * @return Número de voos.
 */
int flights_manager_count(FlightsManager m) {
    return g_hash_table_size(m->flights);
}

/**
 * @brief Obtém a tabela interna de voos (GHashTable).
 *
 * @param m Ponteiro para o gestor.
 * @return Ponteiro para a GHashTable ou NULL se o gestor for inválido.
 */
GHashTable *flights_manager_get_table(FlightsManager m) {
    if (!m) return NULL;
    return m->flights;
}
