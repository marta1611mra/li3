#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include "queries/query2.h"
#include "entities/aircrafts.h"
#include "entities/flights.h"
#include "managers/aircrafts_manager.h"
#include "managers/flights_manager.h"

typedef struct {
    Aircraft aircraft;
    int count;         
} AircraftCount;

// Callback para contar voos por aeronave.
static void count_flights(gpointer key, gpointer value, gpointer user_data) {
    Flight f = value;
    GHashTable *table = user_data;

    if (get_flight_status(f) == Cancelled) return;

    const char *aircraft_id = get_flight_aircraft_id(f);
    if (!aircraft_id) return;

    int *cnt = g_hash_table_lookup(table, aircraft_id);
    if (!cnt) {
        cnt = g_new(int, 1);
        *cnt = 1;
        g_hash_table_insert(table, g_strdup(aircraft_id), cnt);
    } else {
        (*cnt)++;
    }
}

// Função de comparação para ordenar aeronaves.
 
static int compare_aircrafts(const void *a, const void *b) {
    const AircraftCount *x = a;
    const AircraftCount *y = b;

    if (x->count != y->count)
        return (y->count - x->count);

    return strcmp(get_aircraft_id(x->aircraft), get_aircraft_id(y->aircraft));
}

// Executa a Query 2: listar o Top N de aeronaves por número de voos.

Q2Result q2(FlightsManager fm, AircraftsManager am, int N, const char *filter_manufacturer) {
    Q2Result res = { .rows = NULL, .used = 0 };
    if (!fm || !am || N <= 0 ){
        return res;
    } else {

        // Contagem de voos
        GHashTable *flight_counts = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);
        g_hash_table_foreach(flights_manager_get_table(fm), count_flights, flight_counts);

        // Array auxiliar para ordenação
        int total = aircrafts_manager_count(am);
        AircraftCount *array = malloc(sizeof(AircraftCount) * total);
        int used = 0;

        GHashTableIter iter;
        gpointer key, value;
        GHashTable *air_tbl = aircrafts_manager_get_table(am);
        g_hash_table_iter_init(&iter, air_tbl);

        while (g_hash_table_iter_next(&iter, &key, &value)) {
            Aircraft a = value;
            const char *id = get_aircraft_id(a);
            const char *man = get_aircraft_manufacturer(a);

            if (filter_manufacturer && strcmp(man, filter_manufacturer) != 0)
                continue;

            int *cnt = g_hash_table_lookup(flight_counts, id);
            array[used++] = (AircraftCount){ a, cnt ? *cnt : 0 };
        }

        // Ordenação
        qsort(array, used, sizeof(AircraftCount), compare_aircrafts);

        int limit = (N < used ? N : used);
        res.rows = malloc(sizeof(Q2Row) * limit);
        res.used = limit;

        for (int i = 0; i < limit; i++) {
        strcpy(res.rows[i].id, get_aircraft_id(array[i].aircraft));
        strcpy(res.rows[i].manufacturer, get_aircraft_manufacturer(array[i].aircraft));
        strcpy(res.rows[i].model, get_aircraft_model(array[i].aircraft));
        sprintf(res.rows[i].count_str, "%d", array[i].count);
        }

        free(array);
        g_hash_table_destroy(flight_counts);
        return res;
    }
}

void free_q2_result(Q2Result *r) {
    free(r->rows);}

