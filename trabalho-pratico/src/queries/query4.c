#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "query4.h"
#include "dataset.h"
#include "passengers_manager.h"
#include "passengers.h"

typedef struct {
    char *document_number;
    double total_spent;
} PassengerSpending;

// Comparação para ordenar passageiros
static gint compare_passenger_spending(gconstpointer a, gconstpointer b) {
    const PassengerSpending *pa = a;
    const PassengerSpending *pb = b;
    
    if (pa->total_spent > pb->total_spent) return -1;
    if (pa->total_spent < pb->total_spent) return 1;
    return strcmp(pa->document_number, pb->document_number);
}

// Liberta memória de PassengerSpending
static void free_passenger_spending(gpointer data) {
    PassengerSpending *ps = data;
    if (ps) {
        free(ps->document_number);
        free(ps);
    }
}

// Ajusta intervalo para domingo-sábado
static void adjust_to_sunday_saturday(const GDate *begin, const GDate *end, 
                                      GDate *begin_out, GDate *end_out) {
    if (!begin || !end || !begin_out || !end_out) return;
    if (!g_date_valid(begin) || !g_date_valid(end)) return;
    
    *begin_out = *begin;
    *end_out = *end;

    // Ajustar begin para domingo
    GDateWeekday begin_wd = g_date_get_weekday(begin_out);
    if (begin_wd != G_DATE_SUNDAY) {
        guint8 days_back = begin_wd % 7;
        if (days_back > 0) {
            g_date_subtract_days(begin_out, days_back);
        }
    }

    // Ajustar end para sábado
    GDateWeekday end_wd = g_date_get_weekday(end_out);
    if (end_wd != G_DATE_SATURDAY) {
        guint8 days_forward = (7 - end_wd) % 7;
        if (days_forward == 0) days_forward = 6;
        if (days_forward > 0 && days_forward < 7) {
            g_date_add_days(end_out, days_forward);
        }
    }
}

// Contexto para callback de filtro de semanas
typedef struct {
    int use_filter;
    GDate filter_start;
    GDate filter_end;
    GHashTable *top10_counts;
} FilterContext;

// Callback para processar cada semana e extrair top 10
static void process_week(gpointer key, gpointer value, gpointer user_data) {
    const char *week_key = (const char *)key;
    GHashTable *week_spending = (GHashTable *)value;
    FilterContext *ctx = (FilterContext *)user_data;
    
    // Aplicar filtro de datas se necessário
    if (ctx->use_filter) {
        GDate week_date;
        g_date_clear(&week_date, 1);
        g_date_set_parse(&week_date, week_key);
        
        if (!g_date_valid(&week_date) ||
            g_date_compare(&week_date, &ctx->filter_start) < 0 ||
            g_date_compare(&week_date, &ctx->filter_end) > 0) {
            return;
        }
    }
    
    // Converter GHashTable em lista ordenada
    GList *passenger_list = NULL;
    GHashTableIter iter;
    gpointer pkey, pvalue;
    
    g_hash_table_iter_init(&iter, week_spending);
    while (g_hash_table_iter_next(&iter, &pkey, &pvalue)) {
        PassengerSpending *ps = malloc(sizeof(PassengerSpending));
        ps->document_number = strdup((const char *)pkey);
        ps->total_spent = *(double *)pvalue;
        passenger_list = g_list_prepend(passenger_list, ps);
    }
    
    // Ordenar e pegar top 10
    passenger_list = g_list_sort(passenger_list, compare_passenger_spending);
    
    int count = 0;
    for (GList *l = passenger_list; l != NULL && count < 10; l = l->next, count++) {
        PassengerSpending *ps = l->data;
        
        // Incrementar contador deste passageiro
        int *appearances = g_hash_table_lookup(ctx->top10_counts, ps->document_number);
        if (!appearances) {
            appearances = malloc(sizeof(int));
            *appearances = 0;
            g_hash_table_insert(ctx->top10_counts, strdup(ps->document_number), appearances);
        }
        (*appearances)++;
    }
    
    g_list_free_full(passenger_list, free_passenger_spending);
}

// Executa a query 4 
// Usa índice pré-agregado: week_sunday -> (document -> total_price)
void query4_execute(Dataset d, const char *begin_date, const char *end_date, FILE *out) {
    if (!d || !out) {
        if (out) fprintf(out, "\n");
        return;
    }
    
    PassengersManager pm = dataset_get_passengers(d);
    GHashTable *q4_weeks = dataset_get_q4_weeks(d);
    
    if (!pm || !q4_weeks || g_hash_table_size(q4_weeks) == 0) {
        fprintf(out, "\n");
        return;
    }
    
    // Verificar filtro de datas
    int has_begin = (begin_date && strlen(begin_date) > 0);
    int has_end = (end_date && strlen(end_date) > 0);
    
    if (has_begin != has_end) {
        fprintf(out, "\n");
        return;
    }
    
    // Preparar contexto de filtro
    FilterContext ctx;
    ctx.use_filter = 0;
    ctx.top10_counts = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);
    
    if (has_begin && has_end) {
        GDate temp_begin, temp_end;
        g_date_clear(&temp_begin, 1);
        g_date_clear(&temp_end, 1);
        g_date_set_parse(&temp_begin, begin_date);
        g_date_set_parse(&temp_end, end_date);
        
        if (g_date_valid(&temp_begin) && g_date_valid(&temp_end)) {
            g_date_clear(&ctx.filter_start, 1);
            g_date_clear(&ctx.filter_end, 1);
            adjust_to_sunday_saturday(&temp_begin, &temp_end, &ctx.filter_start, &ctx.filter_end);
            
            if (g_date_valid(&ctx.filter_start) && g_date_valid(&ctx.filter_end)) {
                ctx.use_filter = 1;
            }
        }
    }
    
    // Processar todas as semanas em uma única iteração
    g_hash_table_foreach(q4_weeks, process_week, &ctx);
    
    if (g_hash_table_size(ctx.top10_counts) == 0) {
        g_hash_table_destroy(ctx.top10_counts);
        fprintf(out, "\n");
        return;
    }
    
    // Encontrar o passageiro com mais aparições
    const char *best_passenger = NULL;
    int max_count = 0;
    
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, ctx.top10_counts);
    
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        const char *doc = (const char *)key;
        int count = *(int *)value;
        
        if (count > max_count || (count == max_count && (!best_passenger || strcmp(doc, best_passenger) < 0))) {
            best_passenger = doc;
            max_count = count;
        }
    }
    
    // Output
    if (best_passenger) {
        Passenger p = passengers_manager_get(pm, best_passenger);
        if (p) {
            fprintf(out, "%s;%s;%s;%s;%s;%d\n",
                   best_passenger,
                   get_passenger_first_name(p),
                   get_passenger_last_name(p),
                   get_passenger_dob(p),
                   get_passenger_nationality(p),
                   max_count);
        } else { fprintf(out, "\n");}
    } 
        else { fprintf(out, "\n");}

    g_hash_table_destroy(ctx.top10_counts);
}