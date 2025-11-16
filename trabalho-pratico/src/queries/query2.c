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
 * Estrutura auxiliar utilizada para armazenar um avião e o número de voos
 * associados a esse avião.
 */

typedef struct {
    Aircraft aircraft; /** Ponteiro para o avião. */
    int count;         /** Número de voos associados. */
} AircraftCount;

/**
 * Função callback utilizada para contar o número de voos por aeronave.
 *
 * Apenas voos cujo estado **não seja Cancelled** são contabilizados.
 * Os resultados são inseridos numa tabela hash onde:
 *
 * - chave: *aircraft_id* (string)
 * - valor: ponteiro para inteiro contendo o número de voos
 *
 * @param key       Chave (não usada).
 * @param value     Valor, um objeto 'Flight'.
 * @param user_data Ponteiro para a GHashTable onde serão guardados os contadores.
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
 * Função de comparação usada no 'qsort' para ordenar aeronaves.
 *
 * Critérios:
 *  - Número de voos (ordem decrescente)
 *  - ID do avião (ordem alfabética crescente, caso haja empate)
 *
 * @param a Ponteiro para elemento AircraftCount.
 * @param b Ponteiro para elemento AircraftCount.
 * @return Valor <0, 0 ou >0 conforme a ordenação desejada pelo qsort.
 */

static int compare_aircrafts(const void *a, const void *b) {
    const AircraftCount *x = a;
    const AircraftCount *y = b;

    if (x->count != y->count)
        return (y->count - x->count); 

    return strcmp(get_aircraft_id(x->aircraft), get_aircraft_id(y->aircraft));
}

/**
 * Executa a Query 2: listar os **Top N aviões com mais voos registados**.
 *
 * Funcionamento:
 * - Conta todos os voos por aeronave (ignorando voos cancelados).
 * - Opcionalmente filtra aviões por fabricante.
 * - Ordena pela quantidade de voos (desc) e ID (asc).
 * - Imprime no ficheiro de saída no formato:
 *
 *     aircraft_id,manufacturer,model,flight_count
 *
 * Se não existirem aeronaves válidas, imprime uma linha vazia.
 *
 * @param fm                 Estrutura FlightsManager contendo todos os voos.
 * @param am                 Estrutura AircraftsManager contendo todas as aeronaves.
 * @param N                  Número máximo de aeronaves a listar.
 * @param filter_manufacturer Se não for NULL, apenas aviões deste fabricante são considerados.
 * @param out                Ficheiro onde será escrito o resultado.
 */

void query2_topN_aircrafts(FlightsManager fm, AircraftsManager am, int N, const char *filter_manufacturer, FILE *out) {
    if (!fm || !am || N <= 0 || !out){
        fprintf(out,"\n");
    }else{

    // Contar voos por aviao
    GHashTable *flight_counts = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);
    g_hash_table_foreach(flights_manager_get_table(fm), count_flights, flight_counts);

    // Prepara array (prealloc com a contagem total de avioes)
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

        // Filtrar pelo fabricante  
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

    // sort do array (apenas a parte usada)
    qsort(array, used, sizeof(AircraftCount), compare_aircrafts);

    // print top N
    int limit = (N < used ? N : used);
    for (int i = 0; i < limit; i++) {
        fprintf(out, "%s,%s,%s,%d\n",
                get_aircraft_id(array[i].aircraft),
                get_aircraft_manufacturer(array[i].aircraft),
                get_aircraft_model(array[i].aircraft),
                array[i].count);
    }

    // limpa
    free(array);
    g_hash_table_destroy(flight_counts);
    }
}
