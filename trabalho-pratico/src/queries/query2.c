#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include "queries/query2.h"
#include "entities/aircrafts.h"
#include "entities/flights.h"
#include "managers/aircrafts_manager.h"
#include "managers/flights_manager.h"

// Struct used to store results
typedef struct {
    Aircraft aircraft;
    int count;
} AircraftCount;

// Callback: count flights per aircraft (ignore Cancelled)
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

// Sorting: descending count, ascending aircraft_id
static int compare_aircrafts(const void *a, const void *b) {
    const AircraftCount *x = a;
    const AircraftCount *y = b;

    if (x->count != y->count)
        return (y->count - x->count); // descending flight count

    return strcmp(get_aircraft_id(x->aircraft), get_aircraft_id(y->aircraft));
}

// Main function
void query2_topN_aircrafts(FlightsManager fm, AircraftsManager am, int N, const char *filter_manufacturer, FILE *out) {
    if (!fm || !am || N <= 0 || !out){
        fprintf(out,"\n");
    }else{

    // Step 1: count flights per aircraft
    GHashTable *flight_counts = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);
    g_hash_table_foreach(flights_manager_get_table(fm), count_flights, flight_counts);

    // Step 2: prepare array (prealloc with total aircraft count)
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

        // Filter manufacturer if provided
        if (filter_manufacturer && strcmp(man, filter_manufacturer) != 0)
            continue;

        int *cnt = g_hash_table_lookup(flight_counts, id);
        array[used].aircraft = a;
        array[used].count = cnt ? *cnt : 0;
        used++;
    }
    
    if (used == 0) {
        fprintf(out, "\n");
        free(array);
        g_hash_table_destroy(flight_counts);
        return;
    }

    // Step 3: sort array (only the used part)
    qsort(array, used, sizeof(AircraftCount), compare_aircrafts);

    // Step 4: print top N
    int limit = (N < used ? N : used);
    for (int i = 0; i < limit; i++) {
        fprintf(out, "%s,%s,%s,%d\n",
                get_aircraft_id(array[i].aircraft),
                get_aircraft_manufacturer(array[i].aircraft),
                get_aircraft_model(array[i].aircraft),
                array[i].count);
    }

    // Cleanup
    free(array);
    g_hash_table_destroy(flight_counts);
    }
}
