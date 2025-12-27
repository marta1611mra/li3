#include "dataset.h"
#include <stdlib.h>

struct dataset {
    AirportsManager airports;       /** Gestor de aeroportos */
    AircraftsManager aircrafts;     /** Gestor de aeronaves */
    FlightsManager flights;         /** Gestor de voos */
    PassengersManager passengers;   /** Gestor de passageiros */
    ReservationsManager reservations; /** Gestor de reservas */
    GHashTable *q6_index; // nationality -> (airport -> count)
    GHashTable *q2_index;
};

// Cria e inicializa um novo Dataset.
Dataset dataset_create(void) {
    Dataset d = malloc(sizeof(struct dataset));
    if (!d) return NULL;

    d->airports = airports_manager_create();
    d->aircrafts = aircrafts_manager_create();
    d->flights = flights_manager_create();
    d->passengers = passengers_manager_create();
    d->reservations = reservations_manager_create();

    d->q6_index = g_hash_table_new_full(
    g_str_hash,
    g_str_equal,
    free,                           // nationality
    (GDestroyNotify) g_hash_table_destroy);
    d->q2_index = g_hash_table_new_full(
    g_str_hash,
    g_str_equal,
    free,
    (GDestroyNotify) g_hash_table_destroy
);


    return d;
}

// Liberta toda a memória associada ao Dataset.
void dataset_destroy(Dataset d) {
    if (!d) return;

    g_hash_table_destroy(d->q2_index);
    g_hash_table_destroy(d->q6_index);
    airports_manager_destroy(d->airports);
    aircrafts_manager_destroy(d->aircrafts);
    flights_manager_destroy(d->flights);
    passengers_manager_destroy(d->passengers);
    reservations_manager_destroy(d->reservations);
    
    free(d);
}

// Função auxiliar para atualizar uma tabela específica (evita repetição de código)
static void update_q2_inner_table(GHashTable *master_index, const char *key, const char *aircraft_id) {
    // 1. Buscar a tabela interna (do fabricante ou global)
    GHashTable *inner_table = g_hash_table_lookup(master_index, key);

    // 2. Se não existir, criar
    if (!inner_table) {
        inner_table = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);
        g_hash_table_insert(master_index, g_strdup(key), inner_table);
    }

    // 3. Atualizar contagem da aeronave
    int *count = g_hash_table_lookup(inner_table, aircraft_id);
    if (count) {
        (*count)++;
    } else {
        int *new_count = malloc(sizeof(int));
        *new_count = 1;
        g_hash_table_insert(inner_table, g_strdup(aircraft_id), new_count);
    }
}

GHashTable *dataset_get_q2_index(Dataset d) {
    if (!d) return NULL;
    return d->q2_index;
}

// Getter para a tabela da Query 6
GHashTable *dataset_get_q6_index(Dataset d) {
    if (!d) return NULL;
    return d->q6_index;
}

void dataset_update_q2(Dataset d, const char *aircraft_id, const char *manufacturer) {
    if (!d || !aircraft_id || !manufacturer) return;

    // 1. Atualizar na tabela do Fabricante Específico
    update_q2_inner_table(d->q2_index, manufacturer, aircraft_id);

    // 2. Atualizar na tabela Global "__ALL__"
    update_q2_inner_table(d->q2_index, "__ALL__", aircraft_id);
}

void dataset_build_q6_index(Dataset d) {
    d->q6_index = g_hash_table_new_full(
        g_str_hash, g_str_equal, free,
        (GDestroyNotify) g_hash_table_destroy
    );

    GList *reservations = reservations_manager_get_all(d->reservations);

    for (GList *l = reservations; l; l = l->next) {
        Reservation r = l->data;

        const char *doc = get_reservation_document_number(r);
        Passenger p = passengers_manager_get(d->passengers, doc);
        if (!p) continue;

        const char *nat = get_passenger_nationality(p);
        if (!nat) continue;

        GHashTable *by_airport =
            g_hash_table_lookup(d->q6_index, nat);

        if (!by_airport) {
            by_airport = g_hash_table_new_full(
                g_str_hash, g_str_equal, free, free
            );
            g_hash_table_insert(d->q6_index, g_strdup(nat), by_airport);
        }

        for (int i = 0; i < 2; i++) {
            const char *fid = get_reservation_flight_id(r, i);
            if (!fid || !*fid) continue;

            Flight f = flights_manager_get(d->flights, fid);
            if (!f || get_flight_status(f) == Cancelled) continue;

            const char *dest = get_flight_dest(f);
            if (!dest) continue;

            int *cnt = g_hash_table_lookup(by_airport, dest);
            if (!cnt) {
                cnt = malloc(sizeof(int));
                *cnt = 0;
                g_hash_table_insert(by_airport, g_strdup(dest), cnt);
            }
            (*cnt)++;
        }
    }
}

void dataset_update_q6(Dataset d, const char *nationality, const char *airport) {
    if (!d || !nationality || !airport) return;

    // 1. Obter a tabela dessa nacionalidade
    GHashTable *nat_table = g_hash_table_lookup(d->q6_index, nationality);

    // 2. Se não existir tabela para esta nacionalidade, criar uma nova
    if (!nat_table) {
        nat_table = g_hash_table_new_full(
            g_str_hash, 
            g_str_equal, 
            free,   // Liberta a chave (nome do aeroporto)
            free    // Liberta o valor (contador)
        );
        // Inserimos uma cópia da string nacionalidade como chave
        g_hash_table_insert(d->q6_index, g_strdup(nationality), nat_table);
    }

    // 3. Atualizar o contador do aeroporto
    int *count = g_hash_table_lookup(nat_table, airport);
    if (count) {
        (*count)++;
    } else {
        int *new_count = malloc(sizeof(int));
        *new_count = 1;
        // Inserimos uma cópia da string aeroporto como chave
        g_hash_table_insert(nat_table, g_strdup(airport), new_count);
    }
}


// Obtém o gestor de aeroportos do Dataset.
AirportsManager dataset_get_airports(Dataset d) { 
    return d->airports; 
}

// Obtém o gestor de aeronaves do Dataset.
AircraftsManager dataset_get_aircrafts(Dataset d) { 
    return d->aircrafts; 
}

// Obtém o gestor de voos do Dataset.
FlightsManager dataset_get_flights(Dataset d) { 
    return d->flights; 
}

// @brief Obtém o gestor de passageiros do Dataset.
PassengersManager dataset_get_passengers(Dataset d) { 
    return d->passengers; 
}

// Obtém o gestor de reservas do Dataset.
ReservationsManager dataset_get_reservations(Dataset d) { 
    return d->reservations; 
}
