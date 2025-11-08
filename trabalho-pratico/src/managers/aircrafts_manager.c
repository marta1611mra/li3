#include <string.h>
#include "aircrafts_manager.h"
#include "aircrafts.h"
#include <glib.h>
#include <stdlib.h>

struct aircrafts_manager {
    GHashTable *aircrafts; // chave = id (tail number)
};

AircraftsManager aircrafts_manager_create() {
    AircraftsManager m = malloc(sizeof(*m));

    if (!m) return NULL;

    m->aircrafts = g_hash_table_new_full(g_str_hash, g_str_equal, free, (GDestroyNotify)destroy_aircraft);
    
    return m;
}

void aircrafts_manager_destroy(AircraftsManager m) {
    if (!m) return;

    g_hash_table_destroy(m->aircrafts);

    free(m);
}

void aircrafts_manager_add(AircraftsManager m, Aircraft a) {
    g_hash_table_insert(m->aircrafts, strdup(get_aircraft_id(a)), a);
}

Aircraft aircrafts_manager_get(AircraftsManager m, const char *id) {
    return g_hash_table_lookup(m->aircrafts, id);
}

int aircrafts_manager_count(AircraftsManager m) {
    return g_hash_table_size(m->aircrafts);
}

bool aircrafts_manager_exists(AircraftsManager am, const char *aircraft_id) {
    if (!am || !aircraft_id) return false;
    return g_hash_table_contains(am->aircrafts, aircraft_id);
}

GHashTable *aircrafts_manager_get_table(AircraftsManager m) {
    if (!m) return NULL;
    return m->aircrafts;
}