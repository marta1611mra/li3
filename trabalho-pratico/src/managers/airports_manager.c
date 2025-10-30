#include "airports_manager.h"
#include "airports.h"
#include <glib.h>
#include <stdlib.h>
#include <string.h>

struct airports_manager {
    GHashTable *airports;  
};

AirportsManager create_airports_manager() {
    AirportsManager m = malloc(sizeof(*m));
    if (!m) return NULL;
    m->airports = g_hash_table_new_full(g_str_hash, g_str_equal, free, (GDestroyNotify)destroy_airport);
    return m;
}

void destroy_airports_manager(AirportsManager m) {
    if (!m) return;
    g_hash_table_destroy(m->airports);
    free(m);
}

void airports_manager_add(AirportsManager m, Airport a) {
    g_hash_table_insert(m->airports, strdup(get_airport_code(m)), a);
}

Airport airports_manager_get(AirportsManager m, const char *code) {
    return g_hash_table_lookup(m->airports, code);
}

int airports_manager_count(AirportsManager m) {
    return g_hash_table_size(m->airports);
}
