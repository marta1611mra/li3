/**
 * @file airports_manager.c
 * @brief Implementação das funções para gestão de aeroportos (AirportsManager) usando GHashTable.
 *
 * O AirportsManager armazena e gerencia múltiplos aeroportos, permitindo
 * adicionar, consultar e contar aeroportos.
 */

#include <string.h>
#include "airports_manager.h"
#include "airports.h"
#include <glib.h>
#include <stdlib.h>

/**
 * @struct airports_manager
 * @brief Estrutura que representa um gestor de aeroportos.
 */
struct airports_manager {
    GHashTable *airports; /**< Tabela hash com chave = código do aeroporto e valor = ponteiro para Airport. */
};

/**
 * @brief Cria um gestor de aeroportos.
 *
 * @return Ponteiro para o gestor criado ou NULL se falhar a alocação.
 */
AirportsManager airports_manager_create() {
    AirportsManager m = malloc(sizeof(*m));
    if (!m) return NULL;

    m->airports = g_hash_table_new_full(g_str_hash, g_str_equal, free, (GDestroyNotify)destroy_airport);
    return m;
}

/**
 * @brief Destroi um gestor de aeroportos, libertando todos os recursos associados.
 *
 * @param m Ponteiro para o gestor a destruir.
 */
void airports_manager_destroy(AirportsManager m) {
    if (!m) return;
    g_hash_table_destroy(m->airports);
    free(m);
}

/**
 * @brief Adiciona um aeroporto ao gestor.
 *
 * @param m Ponteiro para o gestor.
 * @param a Ponteiro para o aeroporto a adicionar.
 */
void airports_manager_add(AirportsManager m, Airport a) {
    g_hash_table_insert(m->airports, strdup(get_airport_code(a)), a);
}

/**
 * @brief Obtém um aeroporto pelo seu código.
 *
 * @param m Ponteiro para o gestor.
 * @param code Código do aeroporto (IATA, 3 letras).
 * @return Ponteiro para o aeroporto ou NULL se não existir.
 */
Airport airports_manager_get(AirportsManager m, const char *code) {
    return g_hash_table_lookup(m->airports, code);
}

/**
 * @brief Obtém o número de aeroportos no gestor.
 *
 * @param m Ponteiro para o gestor.
 * @return Número de aeroportos.
 */
int airports_manager_count(AirportsManager m) {
    return g_hash_table_size(m->airports);
}