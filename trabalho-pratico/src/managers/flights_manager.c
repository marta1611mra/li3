#include <string.h>
#include "flights_manager.h"
#include "flights.h"
#include <glib.h>
#include <stdlib.h>

// Estrutura que representa um gestor de voos.
struct flights_manager {
    GHashTable *flights; // Tabela hash com chave = ID do voo e valor = ponteiro para Flight.
};

// Cria um gestor de voos.
FlightsManager flights_manager_create() {
    FlightsManager m = malloc(sizeof(*m));
    if (!m) return NULL;

    m->flights = g_hash_table_new_full(g_str_hash, g_str_equal, free, (GDestroyNotify)destroy_flight);
    return m;
}

// Destroi um gestor de voos, libertando todos os recursos associados.
void flights_manager_destroy(FlightsManager m) {
    if (!m) return;
    g_hash_table_destroy(m->flights);
    free(m);
}

// Adiciona um voo ao gestor.
void flights_manager_add(FlightsManager m, Flight f) {
    g_hash_table_insert(m->flights, strdup(get_flight_id(f)), f);
}

// Obtém um voo pelo seu ID.
Flight flights_manager_get(FlightsManager m, const char *id) {
    return g_hash_table_lookup(m->flights, id);
}
