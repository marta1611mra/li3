#include "aircrafts_manager.h"
#include "aircrafts.h"
#include <glib.h>
#include <stdlib.h>
#include <string.h>

struct aircrafts_manager {
    GHashTable *aircrafts; // chave = id (tail number)
};

AircraftsManager create_aircrafts_manager() {
    AircraftsManager m = malloc(sizeof(*m));
    if (!m) return NULL;
    m->aircrafts = g_hash_table_new_full(g_str_hash, g_str_equal, free, (GDestroyNotify)destroy_aircraft);
    return m;
}

void destroy_aircrafts_manager(AircraftsManager m) {
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

bool aircrafts_manager_exists(AircraftsManager m, const char *id){
    if (!m || !id) return false;
    return g_hash_table_contains(m->aircrafts, id);
}
