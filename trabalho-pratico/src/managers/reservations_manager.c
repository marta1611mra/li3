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
    if (!rm || !r) return;

    const char *id = get_reservation_id(r);
    if (!id) return;

    g_hash_table_replace(rm->reservations,strdup(id), r);
}
