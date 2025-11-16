/**
 * @file aircrafts_manager.c
 * @brief Implementação das funções para gestão de aeronaves (AircraftsManager) usando GHashTable.
 *
 * O AircraftsManager armazena e gerencia múltiplas aeronaves, permitindo
 * adicionar, consultar, verificar existência e contar aeronaves.
 */

#include <string.h>
#include "aircrafts_manager.h"
#include "aircrafts.h"
#include <glib.h>
#include <stdlib.h>

/**
 * @struct aircrafts_manager
 * @brief Estrutura que representa um gestor de aeronaves.
 */
struct aircrafts_manager {
    GHashTable *aircrafts; /**< Tabela hash com chave = ID da aeronave e valor = ponteiro para Aircraft. */
};

/**
 * @brief Cria um gestor de aeronaves.
 *
 * @return Ponteiro para o gestor criado ou NULL se falhar a alocação.
 */
AircraftsManager aircrafts_manager_create() {
    AircraftsManager m = malloc(sizeof(*m));

    if (!m) return NULL;

    m->aircrafts = g_hash_table_new_full(g_str_hash, g_str_equal, free, (GDestroyNotify)destroy_aircraft);
    
    return m;
}

/**
 * @brief Destroi um gestor de aeronaves, libertando todos os recursos associados.
 *
 * @param m Ponteiro para o gestor a destruir.
 */
void aircrafts_manager_destroy(AircraftsManager m) {
    if (!m) return;
   
    g_hash_table_destroy(m->aircrafts);
    
    free(m);
}

/**
 * @brief Adiciona uma aeronave ao gestor.
 *
 * @param m Ponteiro para o gestor.
 * @param a Ponteiro para a aeronave a adicionar.
 */
void aircrafts_manager_add(AircraftsManager m, Aircraft a) {
    g_hash_table_insert(m->aircrafts, strdup(get_aircraft_id(a)), a);
}

/**
 * @brief Obtém uma aeronave pelo seu ID.
 *
 * @param m Ponteiro para o gestor.
 * @param id ID da aeronave.
 * @return Ponteiro para a aeronave ou NULL se não existir.
 */
Aircraft aircrafts_manager_get(AircraftsManager m, const char *id) {
    return g_hash_table_lookup(m->aircrafts, id);
}

/**
 * @brief Obtém o número de aeronaves no gestor.
 *
 * @param m Ponteiro para o gestor.
 * @return Número de aeronaves.
 */
int aircrafts_manager_count(AircraftsManager m) {
    return g_hash_table_size(m->aircrafts);
}

/**
 * @brief Verifica se uma aeronave existe no gestor pelo seu ID.
 *
 * @param am Ponteiro para o gestor.
 * @param aircraft_id ID da aeronave.
 * @return true se existir, false caso contrário.
 */
bool aircrafts_manager_exists(AircraftsManager am, const char *aircraft_id) {
    if (!am || !aircraft_id) return false;
    return g_hash_table_contains(am->aircrafts, aircraft_id);
}

/**
 * @brief Obtém a tabela interna de aeronaves (GHashTable).
 *
 * @param m Ponteiro para o gestor.
 * @return Ponteiro para a GHashTable ou NULL se o gestor for inválido.
 */
GHashTable *aircrafts_manager_get_table(AircraftsManager m) {
    if (!m) return NULL;
    return m->aircrafts;
}