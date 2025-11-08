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

// função auxiliar: verifica se uma data está entre dois limites (inclusive)
static bool date_in_range(const char* date, const char* start_date, const char* end_date) {
    return (strcmp(date, start_date) >= 0 && strcmp(date, end_date) <= 0);
}

// função auxiliar: incrementa o contador de partidas de um aeroporto
static void count_departures(gpointer key, gpointer value, gpointer user_data) {
    Flight f = (Flight)value;
    struct {
        GHashTable *table;
        const char *start;
        const char *end;
    } *ctx = user_data;

    if (get_flight_status(f) == Cancelled)
        return;

    const char *dep = get_flight_actual_dep(f);
    if (!dep || !date_in_range(dep, ctx->start, ctx->end))
        return;

    const char *orig = get_flight_orig(f);
    if (!orig) return;

    gpointer existing = g_hash_table_lookup(ctx->table, orig);
    int *count = existing ? (int *)existing : NULL;

    if (count) {
        (*count)++;
    } else {
        int *c = malloc(sizeof(int));
        *c = 1;
        g_hash_table_insert(ctx->table, g_strdup(orig), c);
    }
}

// função principal da query 3
void q3(const char *start_date, const char *end_date,
         FlightsManager flights, AirportsManager airports, FILE *output) {

    if (!flights || !airports || !output) return;

    // tabela para guardar contagens de partidas por aeroporto
    GHashTable *departures = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);

    // contexto para passar ao foreach
    struct {
        GHashTable *table;
        const char *start;
        const char *end;
    } ctx = { departures, start_date, end_date };

    // percorre todos os voos
    g_hash_table_foreach(flights_manager_get_table(flights), count_departures, &ctx);

    // encontrar o aeroporto com mais partidas
    GHashTableIter iter;
    gpointer key, value;
    char *best_code = NULL;
    int max_count = 0;

    g_hash_table_iter_init(&iter, departures);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        char *code = (char *)key;
        int count = *(int *)value;

        if (!best_code || count > max_count || (count == max_count && strcmp(code, best_code) < 0)) {
            best_code = code;
            max_count = count;
        }
    }

    // escrever resultado
    if (best_code) {
        Airport a = airports_manager_get(airports, best_code);
        if (a) {
            fprintf(output, "%s,%s,%s,%s,%d\n",
                    get_airport_code(a),
                    get_airport_name(a),
                    get_airport_city(a),
                    get_airport_country(a),
                    max_count);
        } else {
            fprintf(output, "%s,%d\n", best_code, max_count);
        }
    } else {
        fprintf(output, "\n");
    }

    g_hash_table_destroy(departures);
}
