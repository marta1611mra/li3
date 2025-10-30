#include "flights_manager.h"
#include "flights.h"
#include <glib.h>
#include <stdlib.h>
#include <string.h>

struct flights_manager {
    GHashTable *flights; // chave = flight_id
};

FlightsManager create_flights_manager() {
    FlightsManager m = malloc(sizeof(*m));
    if (!m) return NULL;
    m->flights = g_hash_table_new_full(g_str_hash, g_str_equal, free, (GDestroyNotify)destroy_flight);
    return m;
}

void destroy_flights_manager(FlightsManager m) {
    if (!m) return;
    g_hash_table_destroy(m->flights);
    free(m);
}

void flights_manager_add(FlightsManager m, Flight f) {
    g_hash_table_insert(m->flights, strdup(get_flight_id(m)), f);
}

Flight flights_manager_get(FlightsManager m, const char *id) {
    return g_hash_table_lookup(m->flights, id);
}

int flights_manager_count(FlightsManager m) {
    return g_hash_table_size(m->flights);
}
