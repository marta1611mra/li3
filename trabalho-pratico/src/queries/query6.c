#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include "query6.h"
#include "dataset.h"

void q6(Dataset d, const char *nationality, FILE *out) {
    // Verificações básicas
    if (!d || !nationality || !out) return;

    // 1. Obter o índice principal usando o GETTER (sem d->...)
    GHashTable *q6_index = dataset_get_q6_index(d);
    if (!q6_index) {
        fprintf(out, "\n");
        return;}

    // 2. Procurar a tabela de aeroportos para essa nacionalidade
    GHashTable *airports_map = g_hash_table_lookup(q6_index, nationality);

    // Se não houver dados para a nacionalidade, não faz nada (linha vazia implícita)
    if (!airports_map || g_hash_table_size(airports_map) == 0) {
        fprintf(out, "\n");
        return;
    }

    // 3. Iterar para encontrar o aeroporto com mais passageiros
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, airports_map);

    char *best_airport = NULL;
    int max_count = -1;

    while (g_hash_table_iter_next(&iter, &key, &value)) {
        char *current_airport = (char *)key;
        int current_count = *(int *)value;

        // Se encontrarmos um valor maior, atualizamos
        if (current_count > max_count) {
            max_count = current_count;
            best_airport = current_airport;
        }
        // Se for empate, critério lexicográfico (menor código ganha)
        else if (current_count == max_count) {
            if (best_airport && strcmp(current_airport, best_airport) < 0) {
                best_airport = current_airport;
            }
        }
    }

    // 4. Escrever o resultado: Code;Count
    if (best_airport) {
        fprintf(out, "%s;%d\n", best_airport, max_count);
    }else{
        fprintf(out, "\n");
    }
}