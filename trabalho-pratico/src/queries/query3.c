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

static void count_departures(gpointer key, gpointer value, gpointer user_data) {
    Flight f = (Flight)value;
    struct {
        GHashTable *table;
        const char *start;
        const char *end;
    } *ctx = user_data;

    if (get_flight_status(f) == Cancelled)
        return;

    const char *dep_full = get_flight_actual_dep(f);
    
    if (!dep_full || strlen(dep_full) < 16 || dep_full[10] != ' ') return;

    // ignorar "N/A" e formatos inválidos
    if (strlen(dep_full) < 16 || dep_full[10] != ' ')
        return;
    char dep_date[11];
    memcpy(dep_date, dep_full, 10);
    dep_date[10] = '\0';

    if (strcmp(dep_date, ctx->start) < 0 || strcmp(dep_date, ctx->end) > 0)
        return;

    const char *orig = get_flight_orig(f);
    if (!orig) return;

    int *count = g_hash_table_lookup(ctx->table, orig);
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

    GHashTable *departures = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);

    // contexto 
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

    char final_code[4] = {0};
    int max_count = 0;

    g_hash_table_iter_init(&iter, departures);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        char *code = key;
        int count = *(int *)value;

        if (count > max_count ||
            (count == max_count &&
             (final_code[0] == 0 || strcmp(code, final_code) < 0))) {
            strncpy(final_code, code, 3);
            final_code[3] = '\0';
            max_count = count;
        }
    }
    // escrever resultado
    if (final_code[0] != 0) {
        Airport a = airports_manager_get(airports, final_code);
        if (a) {
            fprintf(output, "%s,%s,%s,%s,%d\n",
                    get_airport_code(a),
                    get_airport_name(a),
                    get_airport_city(a),
                    get_airport_country(a),
                    max_count);} 
        else {fprintf(output, "%s,%d\n", final_code, max_count);}} 
    else {fprintf(output, "\n");
    }
    g_hash_table_destroy(departures);
}
