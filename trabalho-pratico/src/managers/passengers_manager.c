/**
 * @file passengers_manager.c
 * @brief Implementação das funções para gestão de passageiros (PassengersManager) usando GHashTable.
 *
 * O PassengersManager armazena e gerencia múltiplos passageiros, permitindo
 * adicionar, consultar, verificar existência e contar passageiros.
 */

#include <string.h>
#include "passengers_manager.h"
#include "passengers.h"
#include <glib.h>
#include <stdlib.h>
#include <stdbool.h>

/**
 * @struct passengers_manager
 * @brief Estrutura que representa um gestor de passageiros.
 */
struct passengers_manager {
    GHashTable *passengers; /**< Tabela hash com chave = document_id e valor = ponteiro para Passenger. */
};

/**
 * @brief Cria um gestor de passageiros.
 *
 * @return Ponteiro para o gestor criado ou NULL se falhar a alocação.
 */
PassengersManager passengers_manager_create() {
    PassengersManager m = malloc(sizeof(*m));
    if (!m) return NULL;

    m->passengers = g_hash_table_new_full(g_str_hash, g_str_equal, free, (GDestroyNotify)destroy_passenger);
    return m;
}

/**
 * @brief Destroi um gestor de passageiros, libertando todos os recursos associados.
 *
 * @param m Ponteiro para o gestor a destruir.
 */
void passengers_manager_destroy(PassengersManager m) {
    if (!m) return;
    g_hash_table_destroy(m->passengers);
    free(m);
}

/**
 * @brief Adiciona um passageiro ao gestor.
 *
 * @param m Ponteiro para o gestor.
 * @param p Ponteiro para o passageiro a adicionar.
 */
void passengers_manager_add(PassengersManager m, Passenger p) {
    g_hash_table_insert(m->passengers, strdup(get_passenger_document_id(p)), p);
}

/**
 * @brief Obtém um passageiro pelo seu document_id.
 *
 * @param m Ponteiro para o gestor.
 * @param id ID do documento do passageiro.
 * @return Ponteiro para o passageiro ou NULL se não existir.
 */
Passenger passengers_manager_get(PassengersManager m, const char *id) {
    return g_hash_table_lookup(m->passengers, id);
}

/**
 * @brief Obtém o número de passageiros no gestor.
 *
 * @param m Ponteiro para o gestor.
 * @return Número de passageiros.
 */
int passengers_manager_count(PassengersManager m) {
    return g_hash_table_size(m->passengers);
}

/**
 * @brief Verifica se um passageiro existe no gestor pelo seu document_id.
 *
 * @param m Ponteiro para o gestor.
 * @param document_id ID do documento do passageiro.
 * @return true se existir, false caso contrário.
 */
bool passengers_manager_exists(PassengersManager m, const char *document_id) {
    if (!m || !document_id) return false;
    return g_hash_table_contains(m->passengers, document_id);
}
