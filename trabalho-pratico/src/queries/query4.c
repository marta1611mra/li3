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
static void adjust_to_sunday_saturday(const GDate *begin, const GDate *end, GDate *begin_out, GDate *end_out) {
    if (!begin || !end || !begin_out || !end_out) return;
    if (!g_date_valid(begin) || !g_date_valid(end)) return;
    *begin_out = *begin;
    *end_out = *end;

    // 1. Ajustar data inicial para Domingo
    GDateWeekday begin_wd = g_date_get_weekday(begin_out);
    // Se for Domingo (7), 7%7=0 (não mexe). Se for Segunda (1), recua 1.
    guint8 days_back = begin_wd % 7; 
    if (days_back > 0) { 
        g_date_subtract_days(begin_out, days_back);
    }

    // 2. Ajustar data final para Sábado
    GDateWeekday end_wd = g_date_get_weekday(end_out);
    
    // CORREÇÃO CRÍTICA AQUI:
    // Queremos chegar a Sábado (6).
    // Fórmula segura para avançar até ao próximo Sábado:
    // (6 - dia_atual + 7) % 7
    int days_forward = (6 - (int)end_wd + 7) % 7;
    
    if (days_forward > 0) { 
        g_date_add_days(end_out, days_forward);
    }
}

// Contexto para callback de filtro de semanas
typedef struct {
    int use_filter; // indica se o filtro de datas está ativo
    GDate filter_start; // data de início do intervalo de filtro
    GDate filter_end; // data de fim do intervalo de filtro
    GHashTable *top10_counts; // contagem de aparições dos top 10 passageiros
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
        
        // Usar sscanf para garantir leitura correta
        int y, m, d;
        if (sscanf(week_key, "%d-%d-%d", &y, &m, &d) == 3) {
             g_date_set_dmy(&week_date, d, m, y);
        } else {
             return; // data inválida ou formato inesperado
        }
        
        if (!g_date_valid(&week_date)) { return;} // data inválida
        
        GDate week_end = week_date; // domingo da semana
        g_date_add_days(&week_end, 6); // sábado da mesma semana

        // Verificar sobreposição com o intervalo de filtro
        if (g_date_compare(&week_date, &ctx->filter_end) > 0 || g_date_compare(&week_end, &ctx->filter_start) < 0) {
            return;}
    }
    
    if (g_hash_table_size(week_spending) == 0) return; // Verificar se há passageiros nesta semana
    
    // Converter GHashTable em lista ordenada 
    GList *passenger_list = NULL; 
    GHashTableIter iter;
    gpointer pkey, pvalue; 
    
    g_hash_table_iter_init(&iter, week_spending); // Iterar sobre passageiros e seus gastos
    while (g_hash_table_iter_next(&iter, &pkey, &pvalue)) { // pkey: document_number, pvalue: total_spent
        PassengerSpending *ps = malloc(sizeof(PassengerSpending)); 
        if (!ps) continue;
        
        ps->document_number = strdup((const char *)pkey); // duplicar ID do passageiro 
        if (!ps->document_number) { // falha na duplicação
            free(ps);
            continue;
        }
        
        ps->total_spent = *(double *)pvalue; // copiar gasto total
        passenger_list = g_list_prepend(passenger_list, ps); // adicionar à lista 
    }
    
    if (!passenger_list) return; // Nenhum passageiro processado
    
    passenger_list = g_list_sort(passenger_list, compare_passenger_spending); // Ordenar e obter o top 10
    
    int count = 0; // Contador para os top 10
    for (GList *l = passenger_list; l != NULL && count < 10; l = l->next, count++) {
        PassengerSpending *ps = l->data; 
        
        // Incrementar contador deste passageiro
        int *appearances = g_hash_table_lookup(ctx->top10_counts, ps->document_number); 
        if (!appearances) {
            appearances = malloc(sizeof(int)); // alocar novo contador
            if (appearances) {
                *appearances = 1; // primeira aparição
                g_hash_table_insert(ctx->top10_counts, strdup(ps->document_number), appearances); // inserir na tabela 
                }
        } else { (*appearances)++;} 
    } g_list_free_full(passenger_list, free_passenger_spending); 
}

// Executa a query 4 
void query4_execute(Dataset d, const char *begin_date, const char *end_date, FILE *out) {
    if (!d || !out) { 
        if (out) output_empty(out);
        return;
    }
    
    PassengersManager pm = dataset_get_passengers(d); // Obter manager de passageiros
    GHashTable *q4_weeks = dataset_get_q4_weeks(d);  // Obter dados 
    
    if (!pm || !q4_weeks || g_hash_table_size(q4_weeks) == 0) {
        output_empty(out); 
        return;
    }
    
    int has_begin = (begin_date && strlen(begin_date) > 0); // verificar presença de data de início
    int has_end = (end_date && strlen(end_date) > 0); // verificar presença de data de fim
    
    // Ambas as datas devem estar presentes ou ambos ausentes
    if (has_begin != has_end) {
        output_empty(out);
        return;
    }
    
    // Preparar contexto de filtro
    FilterContext ctx; 
    ctx.use_filter = 0;// indicador de uso de filtro de datas
    ctx.top10_counts = g_hash_table_new_full(g_str_hash, g_str_equal, free, free); // tabela para contagem de aparições dos top 10
    
    if (has_begin && has_end) { // se tiver intervalo de datas
        GDate temp_begin, temp_end;
        g_date_clear(&temp_begin, 1); 
        g_date_clear(&temp_end, 1);
        
        int y, m, d;
        int parsed_begin = 0, parsed_end = 0;

        if (sscanf(begin_date, "%d/%d/%d", &y, &m, &d) == 3 || sscanf(begin_date, "%d-%d-%d", &y, &m, &d) == 3) {
            g_date_set_dmy(&temp_begin, d, m, y);
            parsed_begin = 1;
        }
        
        if (sscanf(end_date, "%d/%d/%d", &y, &m, &d) == 3 || sscanf(end_date, "%d-%d-%d", &y, &m, &d) == 3) {
            g_date_set_dmy(&temp_end, d, m, y);
            parsed_end = 1;
        }

        if (parsed_begin && parsed_end && g_date_valid(&temp_begin) && g_date_valid(&temp_end)) { 
            g_date_clear(&ctx.filter_start, 1);
            g_date_clear(&ctx.filter_end, 1);
            adjust_to_sunday_saturday(&temp_begin, &temp_end, &ctx.filter_start, &ctx.filter_end); // usar ajuste para domingo-sábado
            if (g_date_valid(&ctx.filter_start) && g_date_valid(&ctx.filter_end)) {
                ctx.use_filter = 1; // ativar uso de filtro
            }
        } else {
             // Se as datas forem inválidas, o comportamento esperado é geralmente output vazio
             g_hash_table_destroy(ctx.top10_counts);
             output_empty(out);
             return;
        }
    }

    g_hash_table_foreach(q4_weeks, process_week, &ctx); // processar todas as semanas

    if (g_hash_table_size(ctx.top10_counts) == 0) {  
        g_hash_table_destroy(ctx.top10_counts); 
        output_empty(out);
        return;
    }
    
    const char *best_passenger = NULL; // passageiro com mais aparições no top 10
    int max_count = 0; // contador máximo de aparições
    
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, ctx.top10_counts); // iterar sobre contadores de aparições
    
    while (g_hash_table_iter_next(&iter, &key, &value)) { 
        const char *doc = (const char *)key;
        int count = *(int *)value;
        
        // Escolher baseado em: mais aparições OU (empate E menor ID)
        if (count > max_count || (count == max_count && (best_passenger == NULL || strcmp(doc, best_passenger) < 0))) {
            best_passenger = doc; 
            max_count = count;
        }
    }
    
    // Output
    if (best_passenger) {
        Passenger p = passengers_manager_get(pm, best_passenger);
        if (p) {
            char sep = get_output_separator();
            // Proteção contra campos NULL para evitar Segmentation Fault
            const char *fname = get_passenger_first_name(p) ? get_passenger_first_name(p) : "";
            const char *lname = get_passenger_last_name(p) ? get_passenger_last_name(p) : "";
            const char *dob = get_passenger_dob(p) ? get_passenger_dob(p) : "";
            const char *nat = get_passenger_nationality(p) ? get_passenger_nationality(p) : "";

            fprintf(out, "%s%c%s%c%s%c%s%c%s%c%d\n",
                   best_passenger, sep,
                   fname, sep,
                   lname, sep,
                   dob, sep,
                   nat, sep,
                   max_count);
        } else { 
            output_empty(out);
        }
    } else { 
        output_empty(out);
    }

    g_hash_table_destroy(ctx.top10_counts);
}