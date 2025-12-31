#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "query4.h"
#include "dataset.h"
#include "passengers_manager.h"
#include "passengers.h"
#include "output_format.h"

typedef struct {
    char *document_number;
    double total_spent;
} PassengerSpending;

// Comparação para ordenar passageiros (decrescente por gasto, desempate por ID)
static gint compare_passenger_spending(gconstpointer a, gconstpointer b) {
    const PassengerSpending *pa = a;
    const PassengerSpending *pb = b;
    
    // Ordenar por gasto (decrescente)
    if (pa->total_spent > pb->total_spent) return -1;
    if (pa->total_spent < pb->total_spent) return 1;

    // Desempate por ID (crescente/lexicográfico)
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

// Ajusta intervalo para domingo-sábado (início e fim de semana)
static void adjust_to_sunday_saturday(const GDate *begin, const GDate *end, 
                                      GDate *begin_out, GDate *end_out) {
    if (!begin || !end || !begin_out || !end_out) return;
    if (!g_date_valid(begin) || !g_date_valid(end)) return;
    
    *begin_out = *begin;
    *end_out = *end;

    // Ajustar begin para domingo (início da semana)
    GDateWeekday begin_wd = g_date_get_weekday(begin_out);
    if (begin_wd != G_DATE_SUNDAY) {
        // Número de dias para subtrair até domingo
        guint8 days_back = begin_wd % 7;
        if (days_back > 0) {
            g_date_subtract_days(begin_out, days_back);
        }
    }

    // Ajustar end para sábado (fim da semana)
    GDateWeekday end_wd = g_date_get_weekday(end_out);
    if (end_wd != G_DATE_SATURDAY) {
        // Número de dias para adicionar até sábado
        guint8 days_forward = (G_DATE_SATURDAY - end_wd);
        if (days_forward > 0) {
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
        
        if (!g_date_valid(&week_date)) {
            return;
        }

        // A semana deve estar dentro ou sobrepor o intervalo de filtro
        GDate week_end = week_date;
        g_date_add_days(&week_end, 6); // Sábado da mesma semana

        // Verificar se a semana sobrepõe o intervalo
        if (g_date_compare(&week_date, &ctx->filter_end) > 0 ||
            g_date_compare(&week_end, &ctx->filter_start) < 0) {
            return;
        }
    }
    
    // Verificar se há passageiros nesta semana
    if (g_hash_table_size(week_spending) == 0) return;
    
    // Converter GHashTable em lista ordenada
    GList *passenger_list = NULL;
    GHashTableIter iter;
    gpointer pkey, pvalue;
    
    g_hash_table_iter_init(&iter, week_spending);
    while (g_hash_table_iter_next(&iter, &pkey, &pvalue)) {
        PassengerSpending *ps = malloc(sizeof(PassengerSpending));
        if (!ps) continue;
        
        ps->document_number = strdup((const char *)pkey);
        if (!ps->document_number) {
            free(ps);
            continue;
        }
        
        ps->total_spent = *(double *)pvalue;
        passenger_list = g_list_prepend(passenger_list, ps);
    }
    
    if (!passenger_list) return;
    
    // Ordenar e pegar top 10
    passenger_list = g_list_sort(passenger_list, compare_passenger_spending);
    
    int count = 0;
    for (GList *l = passenger_list; l != NULL && count < 10; l = l->next, count++) {
        PassengerSpending *ps = l->data;
        
        // Incrementar contador deste passageiro
        int *appearances = g_hash_table_lookup(ctx->top10_counts, ps->document_number);
        if (!appearances) {
            appearances = malloc(sizeof(int));
            if (appearances) {
                *appearances = 1;
                g_hash_table_insert(ctx->top10_counts, strdup(ps->document_number), appearances);
            }
        } else {
            (*appearances)++;
        }
    }
    
    g_list_free_full(passenger_list, free_passenger_spending);
}

// Executa a query 4 
void query4_execute(Dataset d, const char *begin_date, const char *end_date, FILE *out) {
    if (!d || !out) {
        if (out) output_empty(out);
        return;
    }
    
    PassengersManager pm = dataset_get_passengers(d);
    GHashTable *q4_weeks = dataset_get_q4_weeks(d);
    
    if (!pm || !q4_weeks || g_hash_table_size(q4_weeks) == 0) {
        output_empty(out);
        return;
    }
    
    // Verificar filtro de datas
    int has_begin = (begin_date && strlen(begin_date) > 0);
    int has_end = (end_date && strlen(end_date) > 0);
    
    // Ambos devem estar presentes ou ambos ausentes
    if (has_begin != has_end) {
        output_empty(out);
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
    
    // Processar todas as semanas
    g_hash_table_foreach(q4_weeks, process_week, &ctx);
    
    if (g_hash_table_size(ctx.top10_counts) == 0) {
        g_hash_table_destroy(ctx.top10_counts);
        output_empty(out);
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
        
        // Escolher baseado em: mais aparições OU (empate E menor ID)
        if (count > max_count || 
            (count == max_count && (best_passenger == NULL || strcmp(doc, best_passenger) < 0))) {
            best_passenger = doc;
            max_count = count;
        }
    }
    
    // Output
    if (best_passenger) {
        Passenger p = passengers_manager_get(pm, best_passenger);
        if (p) {
            char sep = get_output_separator();
            fprintf(out, "%s%c%s%c%s%c%s%c%s%c%d\n",
                   best_passenger, sep,
                   get_passenger_first_name(p), sep,
                   get_passenger_last_name(p), sep,
                   get_passenger_dob(p), sep,
                   get_passenger_nationality(p), sep,
                   max_count);
        } else { 
            output_empty(out);
        }
    } else { 
        output_empty(out);
    }

    g_hash_table_destroy(ctx.top10_counts);
}