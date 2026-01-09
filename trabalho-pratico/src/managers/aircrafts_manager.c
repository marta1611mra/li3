#include <string.h>
#include "aircrafts_manager.h"
#include "aircrafts.h"
#include <glib.h>
#include <stdlib.h>

// Estrutura que representa um gestor de aeronaves.
struct aircrafts_manager {
    GHashTable *aircrafts; // Tabela hash com chave = ID da aeronave e valor = ponteiro para Aircraft.
};


// Cria um gestor de aeronaves.
AircraftsManager aircrafts_manager_create() {
    AircraftsManager m = malloc(sizeof(*m));

    if (!m) return NULL;

    m->aircrafts = g_hash_table_new_full(g_str_hash, g_str_equal, free, (GDestroyNotify)destroy_aircraft);
    
    return m;
}


// Destroi um gestor de aeronaves, libertando todos os recursos associados.
void aircrafts_manager_destroy(AircraftsManager m) {
    if (!m) return;
   
    g_hash_table_destroy(m->aircrafts);
    
    free(m);
}


// Adiciona uma aeronave ao gestor.
void aircrafts_manager_add(AircraftsManager m, Aircraft a) {
    g_hash_table_insert(m->aircrafts, strdup(get_aircraft_id(a)), a);
}


// Obtém uma aeronave pelo seu ID.
Aircraft aircrafts_manager_get(AircraftsManager m, const char *id) {
    return g_hash_table_lookup(m->aircrafts, id);
}


// Verifica se uma aeronave existe no gestor pelo seu ID.
bool aircrafts_manager_exists(AircraftsManager am, const char *aircraft_id) {
    if (!am || !aircraft_id) return false;
    return g_hash_table_contains(am->aircrafts, aircraft_id);
}
