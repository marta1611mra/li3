#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <glib.h>
#include "dataset.h"
#include "flights_manager.h"
#include "airports_manager.h"
#include "flights.h"
#include "airports.h"
#include "query3.h"

typedef struct {
    GHashTable *table;
    const char *start;
    const char *end;
} Q3Ctx;


//Função callback usada para contar partidas por aeroporto num intervalo de datas.
static void count_departures(gpointer key, gpointer value, gpointer user_data) {
    Flight f = (Flight)value;
    Q3Ctx *ctx = user_data;

    if (get_flight_status(f) == Cancelled)
        return;

    const char *dep_full = get_flight_actual_dep(f);
    if (!dep_full || strlen(dep_full) < 10) return;
    

    char dep_date[11];
    strncpy(dep_date, dep_full, 10); dep_date[10] = '\0';

    if (strcmp(dep_date, ctx->start) < 0 || strcmp(dep_date, ctx->end) > 0)
        return;

    const char *orig = get_flight_orig(f);
    if (!orig) return;

    int *count = g_hash_table_lookup(ctx->table, orig);
    if (!count) {
        count = g_new(int, 1);
        *count = 1;
        g_hash_table_insert(ctx->table, g_strdup(orig), count);
    } else (*count)++;
}


// Executa a Query 3: encontrar o aeroporto com mais partidas num intervalo de datas especificado.

Q3Result q3(const char *start_date, const char *end_date,
        FlightsManager flights, AirportsManager airports) {

    Q3Result r = { .found = false };

    GHashTable *departures = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);
    Q3Ctx ctx = { departures, start_date, end_date };

    // contar partidas
    g_hash_table_foreach(flights_manager_get_table(flights), count_departures, &ctx);

    // encontrar o aeroporto com mais partidas
    GHashTableIter iter;
    gpointer key, value;

    char best_code[4] = "";
    int max = 0;

    g_hash_table_iter_init(&iter, departures);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
       int count = *(int *)value;
        if (count > max || (count == max && strcmp(key, best_code) < 0)) {
        strncpy(best_code, key, 3);
        best_code[3] = '\0';

            max = count;
        }
    }
    if (best_code[0] != '\0') {
        Airport a = airports_manager_get(airports, best_code);
        if (a) {
            r.found = true;
            strcpy(r.code, best_code);
            strcpy(r.name, get_airport_name(a));
            strcpy(r.city, get_airport_city(a));
            strcpy(r.country, get_airport_country(a));
            sprintf(r.count_str, "%d", max);
        }
    }

    g_hash_table_destroy(departures);
    return r;
}
//utilizar o parsing aqui tambem
