/**
 * @file query2.c
 * @brief Implementação da Query 2: Top N aeronaves com mais voos.
 *
 * Este módulo fornece a função para executar a Query 2, que lista as N
 * aeronaves com maior número de voos registados, podendo opcionalmente
 * filtrar por fabricante.
 *
 * O resultado é escrito em ficheiro, cada linha com:
 * 
 *      aircraft_id,manufacturer,model,flight_count
 *
 * Se não houver aeronaves válidas, é escrita uma linha vazia.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include "queries/query2.h"
#include "entities/aircrafts.h"
#include "entities/flights.h"
#include "managers/aircrafts_manager.h"
#include "managers/flights_manager.h"

/**
 * @struct AircraftCount
 * @brief Estrutura auxiliar para armazenar uma aeronave e a contagem de voos.
 */
typedef struct {
    Aircraft aircraft; /**< Ponteiro para a aeronave */
    int count;         /**< Número de voos associados */
} AircraftCount;

/**
 * @brief Callback para contar voos por aeronave.
 *
 * Apenas voos cujo estado não seja Cancelled são contabilizados.
 * Os resultados são guardados numa GHashTable com:
 *   - chave: aircraft_id
 *   - valor: ponteiro para inteiro com o número de voos
 *
 * @param key Chave da tabela (não usada)
 * @param value Ponteiro para o voo (Flight)
 * @param user_data Ponteiro para a GHashTable onde são guardados os contadores
 */
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

/**
 * @brief Função de comparação para ordenar aeronaves.
 *
 * Critérios de ordenação:
 *  1. Número de voos (decrescente)
 *  2. ID da aeronave (ascendente) em caso de empate
 *
 * @param a Ponteiro para um AircraftCount
 * @param b Ponteiro para outro AircraftCount
 * @return Inteiro <0, 0 ou >0 para uso em qsort
 */
static int compare_aircrafts(const void *a, const void *b) {
    const AircraftCount *x = a;
    const AircraftCount *y = b;

    if (x->count != y->count)
        return (y->count - x->count);

    return strcmp(get_aircraft_id(x->aircraft), get_aircraft_id(y->aircraft));
}

/**
 * @brief Executa a Query 2: listar o Top N de aeronaves por número de voos.
 *
 * Passos:
 * 1. Conta todos os voos por aeronave, ignorando voos cancelados
 * 2. Opcionalmente filtra aeronaves pelo fabricante
 * 3. Ordena pelo número de voos (desc) e ID (asc)
 * 4. Imprime o top N no ficheiro de saída
 *
 * @param fm Estrutura FlightsManager com todos os voos
 * @param am Estrutura AircraftsManager com todas as aeronaves
 * @param N Número máximo de aeronaves a listar
 * @param filter_manufacturer Se não NULL, filtra apenas por este fabricante
 * @param out Ficheiro onde será escrito o resultado
 */
void query2_topN_aircrafts(FlightsManager fm, AircraftsManager am, int N, const char *filter_manufacturer, FILE *out) {
    if (!fm || !am || N <= 0 || !out){
        fprintf(out,"\n");
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

        // Ordenação
        qsort(array, used, sizeof(AircraftCount), compare_aircrafts);

        // Impressão do top N
        int limit = (N < used ? N : used);
        for (int i = 0; i < limit; i++) {
            fprintf(out, "%s,%s,%s,%d\n",
                    get_aircraft_id(array[i].aircraft),
                    get_aircraft_manufacturer(array[i].aircraft),
                    get_aircraft_model(array[i].aircraft),
                    array[i].count);
        }

        free(array);
        g_hash_table_destroy(flight_counts);
    }
}
