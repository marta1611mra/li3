#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <glib.h>
#include "dataset.h"
#include "airports_manager.h"
#include "airports.h"
#include "flights_manager.h"
#include "flights.h"
#include "query3.h"
#include "output_format.h"

// Callback para processar cada voo e contar partidas
typedef struct {
    const char *start_date;
    const char *end_date;
    GHashTable *airport_counts;  // airport_code -> count
} Q3Context;

// Conta as partidas de voos não cancelados por aeroporto dentro do intervalo de datas
static void count_flight_departures(gpointer key, gpointer value, gpointer user_data) {
    (void)key; 
    Flight f = (Flight)value;
    Q3Context *ctx = (Q3Context *)user_data;
    
    if (get_flight_status(f) == Cancelled) {     // Ignorar voos cancelados
        return;
    }
    
    const char *actual_dep = get_flight_actual_dep(f);     // Obter data real de partida
    if (!actual_dep || strlen(actual_dep) < 10) {
        return;
    }
    
    // Extrair apenas a data (YYYY-MM-DD)
    char date_only[11];
    memcpy(date_only, actual_dep, 10);
    date_only[10] = '\0';

    if (strcmp(date_only, ctx->start_date) < 0 || 
    strcmp(date_only, ctx->end_date) > 0) {  // Verificar se está no intervalo
        return;    
    }
    
    const char *orig = get_flight_orig(f);   // Obtem aeroporto de origem
    if (!orig || strlen(orig) == 0) {
        return;
    }
    
    int *count = g_hash_table_lookup(ctx->airport_counts, orig);   
    if (count) {
        (*count)++; 
    } else {
        int *new_count = malloc(sizeof(int));
        if (new_count) {
            *new_count = 1;
            g_hash_table_insert(ctx->airport_counts, g_strdup(orig), new_count);
        }
    }
}

// Executa a Query 3: encontrar o aeroporto com mais partidas num intervalo de datas
void q3(Dataset d, char *args[], FILE *output) {
    if (!d || !args || !args[0] || !args[1] || !output) {
        output_empty(output);
        return;
    }

    const char *start_date = args[0];
    const char *end_date = args[1];

    FlightsManager fm = dataset_get_flights(d);     // Obter manager de voos
    if (!fm) {
        output_empty(output);
        return;
    }

    // Criar contexto e tabela de contagem
    Q3Context ctx;
    ctx.start_date = start_date;
    ctx.end_date = end_date;
    ctx.airport_counts = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);

    // Processar todos os voos
    GHashTable *flights_table = flights_manager_get_table(fm);
    if (flights_table) {
        g_hash_table_foreach(flights_table, count_flight_departures, &ctx);
    }

    // Encontrar aeroporto com mais partidas
    char best_airport[4] = {0};
    int max_count = 0;

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, ctx.airport_counts);

    while (g_hash_table_iter_next(&iter, &key, &value)) {
        const char *airport_code = (const char *)key;
        int count = *(int *)value;

        // Atualizar se: mais partidas OU (mesmo número e código menor)
        if (count > max_count ||
            (count == max_count && (best_airport[0] == 0 || strcmp(airport_code, best_airport) < 0))) {
            strncpy(best_airport, airport_code, 3);
            best_airport[3] = '\0';
            max_count = count;
        }
    }

    // Limpar tabela de contagem
    g_hash_table_destroy(ctx.airport_counts);

    // Output do resultado
    if (best_airport[0] != 0 && max_count > 0) {
        AirportsManager am = dataset_get_airports(d);
        Airport a = airports_manager_get(am, best_airport);
        
        char sep = get_output_separator();

        if (a) {
            fprintf(output, "%s%c%s%c%s%c%s%c%d\n",
                    get_airport_code(a), sep,
                    get_airport_name(a), sep,
                    get_airport_city(a), sep,
                    get_airport_country(a), sep,
                    max_count);
        } else {
            // Aeroporto não encontrado (improvável)
            fprintf(output, "%s%c%d\n", best_airport, sep, max_count);
        }
    } else {
        output_empty(output);
    }
}