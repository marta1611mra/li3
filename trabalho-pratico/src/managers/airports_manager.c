#include <string.h>
#include "airports_manager.h"
#include "airports.h"
#include <glib.h>
#include <stdlib.h>


// Estrutura que representa um gestor de aeroportos.
struct airports_manager {
    GHashTable *airports; // Tabela hash com chave = código do aeroporto e valor = ponteiro para Airport. 
};


// Cria um gestor de aeroportos.
AirportsManager airports_manager_create() {
    AirportsManager m = malloc(sizeof(*m));
    if (!m) return NULL;

    m->airports = g_hash_table_new_full(g_str_hash, g_str_equal, free, (GDestroyNotify)destroy_airport);
    return m;
}


// Destroi um gestor de aeroportos, libertando todos os recursos associados.
void airports_manager_destroy(AirportsManager m) {
    if (!m) return;
    g_hash_table_destroy(m->airports);
    free(m);
}


// Adiciona um aeroporto ao gestor.
void airports_manager_add(AirportsManager m, Airport a) {
    g_hash_table_insert(m->airports, strdup(get_airport_code(a)), a);
}


// Obtém um aeroporto pelo seu código.
Airport airports_manager_get(AirportsManager m, const char *code) {
    return g_hash_table_lookup(m->airports, code);
}

// Obtém o número de aeroportos no gestor.
int airports_manager_count(AirportsManager m) {
    return g_hash_table_size(m->airports);
}