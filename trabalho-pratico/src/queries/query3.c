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


/**
 * @brief Função callback usada para contar partidas por aeroporto num intervalo de datas.
 *
 * Esta função é chamada para cada voo armazenado no `FlightsManager`.  
 * Apenas voos **não cancelados** são considerados.
 *
 * O procedimento é:
 * - extrair a data (AAAA-MM-DD) da actual departure do voo;
 * - verificar se a data está dentro do intervalo `[start, end]`;
 * - incrementar o contador associado ao aeroporto de origem.
 *
 * Os resultados são guardados numa GHashTable onde:
 * - chave → código de aeroporto (string)
 * - valor → inteiro alocado dinamicamente contendo o número de partidas
 *
 * @param key        Chave da tabela (não usada).
 * @param value      Valor da tabela, um objeto `Flight`.
 * @param user_data  Estrutura de contexto contendo:
 *                   - tabela de contagens
 *                   - start_date
 *                   - end_date
 */

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


/**
 * @brief Executa a Query 3: encontrar o aeroporto com mais partidas
 *        num intervalo de datas especificado.
 *
 * A query recebe duas datas:
 *
 *     start_date end_date
 *
 * Exemplo:
 *     2022-05-01 2022-05-31
 *
 * A função:
 * 1. Percorre todos os voos do sistema.
 * 2. Considera apenas voos:
 *      - não cancelados
 *      - cuja actual departure esteja dentro do intervalo de datas
 * 3. Conta partidas por aeroporto.
 * 4. Identifica o aeroporto com maior número de partidas.
 * 5. Em caso de empate, usa ordenação lexicográfica do código do aeroporto.
 *
 * O output segue o formato:
 *
 *     code,name,city,country,count
 *
 * Se nenhum aeroporto tiver partidas no intervalo, imprime uma linha vazia.
 *
 * @param start_date  Data inicial (formato YYYY-MM-DD).
 * @param end_date    Data final (formato YYYY-MM-DD).
 * @param flights     Estrutura FlightsManager com todos os voos.
 * @param airports    Estrutura AirportsManager com todos os aeroportos.
 * @param output      Ficheiro onde será escrito o resultado.
 */

void q3(const char *start_date, const char *end_date,
        FlightsManager flights, AirportsManager airports, FILE *output) {

    if (!flights || !airports || !output) return;

    GHashTable *departures = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);

    // contexto de iteração
    struct {
        GHashTable *table;
        const char *start;
        const char *end;
    } ctx = { departures, start_date, end_date };

    // contar partidas
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
