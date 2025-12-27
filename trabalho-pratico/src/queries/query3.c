#include <stdio.h>
#include <string.h>
#include <glib.h>
#include "queries/query3.h"
#include "dataset.h"
#include "output_format.h"

// Estrutura auxiliar para manter o melhor aeroporto 
typedef struct {
    const char *best_airport;
    int best_count;
    const char *date_start;
    const char *date_end;
    int current_count;  // Contador temporário para o aeroporto atual
} Q3Context;

// Callback interno: soma voos num range de datas 
static void q3_sum_dates_cb(gpointer key, gpointer value, gpointer user_data) {
    const char *date = key;
    int count = GPOINTER_TO_INT(value);
    Q3Context *ctx = user_data;

    if (strcmp(date, ctx->date_start) >= 0 &&
        strcmp(date, ctx->date_end)   <= 0) {
        ctx->current_count += count;
    }
}

// Callback principal: percorre aeroportos 
static void q3_airport_cb(gpointer key, gpointer value, gpointer user_data) {
    const char *airport = key;
    GHashTable *dates = value;
    Q3Context *ctx = user_data;

    // Resetar contador do aeroporto atual
    ctx->current_count = 0;

    // Calcular total de partidas deste aeroporto no intervalo
    g_hash_table_foreach(dates, q3_sum_dates_cb, ctx);

    // Atualizar melhor aeroporto se necessário
    if (ctx->current_count > ctx->best_count) {
        ctx->best_count = ctx->current_count;
        ctx->best_airport = airport;
    } else if (ctx->current_count == ctx->best_count && ctx->best_airport) {
        // Em caso de empate, escolher o menor lexicograficamente
        if (strcmp(airport, ctx->best_airport) < 0) {
            ctx->best_airport = airport;
        }
    } else if (ctx->current_count > 0 && !ctx->best_airport) {
        // Primeiro aeroporto com partidas
        ctx->best_count = ctx->current_count;
        ctx->best_airport = airport;
    }
}

// Executa a query 3
void q3(Dataset d, char **args, FILE *output) {

    const char *date_start = args[0];
    const char *date_end   = args[1];

    Q3Context ctx = {
        .best_airport = NULL,
        .best_count   = 0,
        .date_start   = date_start,
        .date_end     = date_end,
        .current_count = 0
    };

    GHashTable *q3_index = dataset_get_q3_index(d);
    if (!q3_index) {
        output_empty(output);
        return;
    }

    g_hash_table_foreach(q3_index, q3_airport_cb, &ctx);

    if (ctx.best_airport) {
        fprintf(output, "%s;%d\n", ctx.best_airport, ctx.best_count);
    } else {
        output_empty(output);
    }
}