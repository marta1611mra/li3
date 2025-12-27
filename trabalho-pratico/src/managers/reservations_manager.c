#include <string.h>
#include "reservations_manager.h"
#include "reservations.h"
#include <glib.h>
#include <stdlib.h>


// Estrutura que representa um gestor de reservas.
struct reservations_manager {
    GHashTable *reservations; /**< Tabela hash com chave = reservation_id e valor = ponteiro para Reservation. */
};

// Cria um gestor de reservas.
ReservationsManager reservations_manager_create() {
    ReservationsManager m = malloc(sizeof(*m));
    if (!m) return NULL;

    m->reservations = g_hash_table_new_full(g_str_hash, g_str_equal, free, (GDestroyNotify)destroy_reservation);
    return m;
}

// Destroi um gestor de reservas, libertando todos os recursos associados.
void reservations_manager_destroy(ReservationsManager m) {
    if (!m) return;
    g_hash_table_destroy(m->reservations);
    free(m);
}

// Adiciona uma reserva ao gestor.
void reservations_manager_add(ReservationsManager rm, Reservation r) {
    const char *id = get_reservation_id(r);

    Reservation old = g_hash_table_lookup(rm->reservations, id);
    if (old) {
        destroy_reservation(old);
        g_hash_table_remove(rm->reservations, id);
    }

    g_hash_table_insert(rm->reservations, g_strdup(id), r);
}


// Obtém uma reserva pelo seu ID.
Reservation reservations_manager_get(ReservationsManager m, const char *id) {
    return g_hash_table_lookup(m->reservations, id);
}

// Obtém o número de reservas no gestor.
int reservations_manager_count(ReservationsManager m) {
    return g_hash_table_size(m->reservations);
}
// Obtém uma lista de todas as reservas no gestor.
GList *reservations_manager_get_all(ReservationsManager m) {
    return g_hash_table_get_values(m->reservations);
}
