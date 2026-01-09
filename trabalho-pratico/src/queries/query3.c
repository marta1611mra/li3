#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <glib.h>
#include "dataset.h"
#include "airports_manager.h"
#include "airports.h"
#include "query3.h"
#include "output_format.h"

// Estrutura para contar partidas por aeroporto
typedef struct {
    char airport_code[4];
    int total_count;
} AirportCount;

// Executa a Query 3 
void q3(Dataset d, char *args[], FILE *output) {
    if (!d || !args || !args[0] || !args[1] || !output) {
        output_empty(output);
        return;
    }

    const char *start_date = args[0];
    const char *end_date = args[1];

    // Obtém o índice pré-computado: airport -> (date -> count)
    GHashTable *q3_index = dataset_get_q3_index(d);
    if (!q3_index) {
        output_empty(output);
        return;
    }

    // Tabela para acumular contagens por aeroporto
    GHashTable *airport_totals = g_hash_table_new_full(
        g_str_hash, g_str_equal, g_free, g_free
    );

    // Itera sobre todos os aeroportos no índice
    GHashTableIter airport_iter;
    gpointer airport_key, date_table_value;
    
    g_hash_table_iter_init(&airport_iter, q3_index);
    while (g_hash_table_iter_next(&airport_iter, &airport_key, &date_table_value)) {
        const char *airport_code = (const char *)airport_key;
        GHashTable *date_table = (GHashTable *)date_table_value;
        
        int airport_total = 0;
        
        // Itera sobre todas as datas deste aeroporto
        GHashTableIter date_iter;
        gpointer date_key, count_value;
        
        g_hash_table_iter_init(&date_iter, date_table);
        while (g_hash_table_iter_next(&date_iter, &date_key, &count_value)) {
            const char *date = (const char *)date_key;
            int count = GPOINTER_TO_INT(count_value);
            
            // Verifica se a data está no intervalo
            if (strcmp(date, start_date) >= 0 && strcmp(date, end_date) <= 0) {
                airport_total += count;
            }
        }
        
        // Se este aeroporto teve partidas no intervalo, adiciona ao total
        if (airport_total > 0) {
            int *total_ptr = g_malloc(sizeof(int));
            *total_ptr = airport_total;
            g_hash_table_insert(airport_totals, g_strdup(airport_code), total_ptr);
        }
    }

    // Encontra o aeroporto com mais partidas
    char best_airport[4] = {0};
    int max_count = 0;

    GHashTableIter totals_iter;
    gpointer key, value;
    g_hash_table_iter_init(&totals_iter, airport_totals);

    while (g_hash_table_iter_next(&totals_iter, &key, &value)) {
        const char *airport_code = (const char *)key;
        int count = *(int *)value;

        // Atualiza se: tiver mais partidas OU mesmo número e código menor
        if (count > max_count ||
            (count == max_count && (best_airport[0] == 0 || strcmp(airport_code, best_airport) < 0))) {
            strncpy(best_airport, airport_code, 3);
            best_airport[3] = '\0';
            max_count = count;
        }
    }

    // Limpa tabela de totais
    g_hash_table_destroy(airport_totals);

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
            // Aeroporto não encontrado no manager
            fprintf(output, "%s%c%d\n", best_airport, sep, max_count);
        }
    } else {
        output_empty(output); // Nenhum voo no intervalo
    }
}