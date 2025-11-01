#include "passengers_manager.h"
#include "passengers.h"
#include <glib.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct passengers_manager {
    GHashTable *passengers; // chave = document_id
};

PassengersManager create_passengers_manager() {
    PassengersManager m = malloc(sizeof(*m));
    if (!m) return NULL;
    m->passengers = g_hash_table_new_full(g_str_hash, g_str_equal, free, (GDestroyNotify)destroy_passenger);
    return m;
}

void destroy_passengers_manager(PassengersManager m) {
    if (!m) return;
    g_hash_table_destroy(m->passengers);
    free(m);
}

void passengers_manager_add(PassengersManager m, Passenger p) {
    g_hash_table_insert(m->passengers, strdup(get_passenger_document_id(m)), p);
}

Passenger passengers_manager_get(PassengersManager m, const char *id) {
    return g_hash_table_lookup(m->passengers, id);
}

int passengers_manager_count(PassengersManager m) {
    return g_hash_table_size(m->passengers);
}

bool passengers_manager_exists(PassengersManager m, const char *document_id) {
    if (!m || !document_id) return false;
    return g_hash_table_contains(m->passengers, document_id);
}
