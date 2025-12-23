#include <string.h>
#include "passengers_manager.h"
#include "passengers.h"
#include <glib.h>
#include <stdlib.h>
#include <stdbool.h>

// Estrutura que representa um gestor de passageiros.
struct passengers_manager {
    GHashTable *passengers; /**< Tabela hash com chave = document_id e valor = ponteiro para Passenger. */
};

// Cria um gestor de passageiros.
PassengersManager passengers_manager_create() {
    PassengersManager m = malloc(sizeof(*m));
    if (!m) return NULL;

    m->passengers = g_hash_table_new_full(g_str_hash, g_str_equal, free, (GDestroyNotify)destroy_passenger);
    return m;
}

// Destroi um gestor de passageiros, libertando todos os recursos associados.
void passengers_manager_destroy(PassengersManager m) {
    if (!m) return;
    g_hash_table_destroy(m->passengers);
    free(m);
}

// Adiciona um passageiro ao gestor.
void passengers_manager_add(PassengersManager m, Passenger p) {
    g_hash_table_insert(m->passengers, strdup(get_passenger_document_id(p)), p);
}

// Obtém um passageiro pelo seu document_id.
Passenger passengers_manager_get(PassengersManager m, const char *id) {
    return g_hash_table_lookup(m->passengers, id);
}

// Obtém o número de passageiros no gestor.
int passengers_manager_count(PassengersManager m) {
    return g_hash_table_size(m->passengers);
}

// Verifica se um passageiro existe no gestor pelo seu document_id.
bool passengers_manager_exists(PassengersManager m, const char *document_id) {
    if (!m || !document_id) return false;
    return g_hash_table_contains(m->passengers, document_id);
}
// Obtém uma lista de todos os passageiros no gestor.
GList *passengers_manager_get_all(PassengersManager m) {
    return g_hash_table_get_values(m->passengers);
}

