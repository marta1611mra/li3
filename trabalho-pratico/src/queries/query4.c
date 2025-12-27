#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "query4.h"
#include "dataset.h"
#include "passengers_manager.h"
#include "reservations_manager.h"
#include "flights_manager.h"
#include "passengers.h"
#include "reservations.h"
#include "flights.h"

typedef struct {
    char *document_number;
    double total_spent;
} PassengerSpending;

// Função de comparação para ordenar passageiros por gasto total (decrescente) e por número de documento (crescente).
static gint compare_passenger_spending(gconstpointer a, gconstpointer b) {
    const PassengerSpending *pa = a;
    const PassengerSpending *pb = b;
    
    if (pa->total_spent > pb->total_spent) return -1;
    if (pa->total_spent < pb->total_spent) return 1;
    return strcmp(pa->document_number, pb->document_number);
}

// Ajusta as datas para o domingo mais próximo (início) e sábado mais próximo (fim).
static void adjust_to_sunday_saturday(const GDate *begin, const GDate *end, 
                                      GDate *begin_out, GDate *end_out) {
    if (!begin || !end || !begin_out || !end_out) return;
    if (!g_date_valid(begin) || !g_date_valid(end)) return;
    
    *begin_out = *begin;
    *end_out = *end;

    // Ajustar begin para domingo
    GDateWeekday begin_wd = g_date_get_weekday(begin_out);
    if (begin_wd != G_DATE_SUNDAY) {
        guint8 days_back = (begin_wd == G_DATE_SUNDAY) ? 0 : begin_wd % 7;
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

// Obtém o domingo da semana da data fornecida.
static void get_week_sunday(const GDate *date, GDate *sunday_out) {
    if (!date || !sunday_out) return;
    if (!g_date_valid(date)) return;
    
    *sunday_out = *date;
    GDateWeekday wd = g_date_get_weekday(sunday_out);
    
    if (wd != G_DATE_SUNDAY) {
        guint8 days_back = (wd == G_DATE_SUNDAY) ? 0 : wd % 7;
        if (days_back > 0 && days_back < 7) {
            g_date_subtract_days(sunday_out, days_back);
        }
    }
}

// Liberta a memória associada a PassengerSpending.
static void free_passenger_spending(gpointer data) {
    PassengerSpending *ps = data;
    if (ps) {
        free(ps->document_number);
        free(ps);
    }
}

// Obtém os top N passageiros por gasto total a partir de uma lista de reservas.
static GList *get_top_n_passengers(GList *reservations, int n) {
    if (!reservations) return NULL;
    
    GHashTable *spending = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);
    
    for (GList *l = reservations; l != NULL; l = l->next) {
        Reservation r = l->data;
        const char *doc = get_reservation_document_number(r);
        if (!doc) continue;
        
        double price = get_reservation_total_price(r);
        double *current = g_hash_table_lookup(spending, doc);
        
        if (!current) {
            current = malloc(sizeof(double));
            *current = 0.0;
            g_hash_table_insert(spending, strdup(doc), current);
        }
        *current += price;
    }
    
    GList *passenger_list = NULL;
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, spending);
    
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        PassengerSpending *ps = malloc(sizeof(PassengerSpending));
        ps->document_number = strdup(key);
        ps->total_spent = *(double *)value;
        passenger_list = g_list_prepend(passenger_list, ps);
    }
    
    g_hash_table_destroy(spending);
    passenger_list = g_list_sort(passenger_list, compare_passenger_spending);
    
    GList *top_n = NULL;
    int count = 0;
    for (GList *l = passenger_list; l != NULL && count < n; l = l->next, count++) {
        PassengerSpending *ps = l->data;
        PassengerSpending *copy = malloc(sizeof(PassengerSpending));
        copy->document_number = strdup(ps->document_number);
        copy->total_spent = ps->total_spent;
        top_n = g_list_append(top_n, copy);
    }
    
    g_list_free_full(passenger_list, free_passenger_spending);
    return top_n;
}

// Executa a query 4.
void query4_execute(Dataset d, const char *begin_date, const char *end_date, FILE *out) {
    if (!d || !out) {
        if (out) fprintf(out, "\n");
        return;
    }
    
    ReservationsManager rm = dataset_get_reservations(d);
    FlightsManager fm = dataset_get_flights(d);
    PassengersManager pm = dataset_get_passengers(d);
    
    if (!rm || !fm || !pm) {
        fprintf(out, "\n");
        return;
    }
    
    GList *all_reservations = reservations_manager_get_all(rm);
    if (!all_reservations) {
        fprintf(out, "\n");
        return;
    }
    
    // Verificar datas
    int has_begin = (begin_date && strlen(begin_date) > 0);
    int has_end = (end_date && strlen(end_date) > 0);
    
    if (has_begin != has_end) {
        fprintf(out, "\n");
        return;
    }
    
    int use_filter = 0;
    GDate filter_start, filter_end;
    
    if (has_begin && has_end) {
        GDate temp_begin, temp_end;
        g_date_clear(&temp_begin, 1);
        g_date_clear(&temp_end, 1);
        g_date_set_parse(&temp_begin, begin_date);
        g_date_set_parse(&temp_end, end_date);
        
        if (g_date_valid(&temp_begin) && g_date_valid(&temp_end)) {
            g_date_clear(&filter_start, 1);
            g_date_clear(&filter_end, 1);
            adjust_to_sunday_saturday(&temp_begin, &temp_end, &filter_start, &filter_end);
            
            if (g_date_valid(&filter_start) && g_date_valid(&filter_end)) {
                use_filter = 1;
            }
        }
    }
    
    // Agrupar por semana
    GHashTable *weeks = g_hash_table_new_full(g_str_hash, g_str_equal, 
                                               free, (GDestroyNotify)g_list_free);
    
    for (GList *l = all_reservations; l != NULL; l = l->next) {
        Reservation r = l->data;
        if (!r) continue;
        
        const char *flight_id = get_reservation_flight_id(r, 0);
        if (!flight_id) continue;
        
        Flight f = flights_manager_get(fm, flight_id);
        if (!f) continue;
        
        const char *dep_str = get_flight_dep(f);
        if (!dep_str) continue;
        
        // Extrair apenas a parte da data (YYYY-MM-DD)
        char date_only[11];
        if (strlen(dep_str) < 10) continue;
        strncpy(date_only, dep_str, 10);
        date_only[10] = '\0';
        
        GDate dep_date;
        g_date_clear(&dep_date, 1);
        g_date_set_parse(&dep_date, date_only);
        if (!g_date_valid(&dep_date)) continue;
        
        GDate week_sunday;
        g_date_clear(&week_sunday, 1);
        get_week_sunday(&dep_date, &week_sunday);
        
        if (!g_date_valid(&week_sunday)) continue;
        
        if (use_filter) {
            if (g_date_compare(&week_sunday, &filter_start) < 0 ||
                g_date_compare(&week_sunday, &filter_end) > 0) {
                continue;
            }
        }
        
        char week_key[12];
        g_date_strftime(week_key, sizeof(week_key), "%Y-%m-%d", &week_sunday);
        
        GList *week_list = g_hash_table_lookup(weeks, week_key);
        week_list = g_list_prepend(week_list, r);
        g_hash_table_replace(weeks, strdup(week_key), week_list);
    }
    
    if (g_hash_table_size(weeks) == 0) {
        g_hash_table_destroy(weeks);
        fprintf(out, "\n");
        return;
    }
    
    // Contar top 10
    GHashTable *top10_counts = g_hash_table_new_full(g_str_hash, g_str_equal, 
                                                      free, free);
    
    GHashTableIter week_iter;
    gpointer key, value;
    g_hash_table_iter_init(&week_iter, weeks);
    
    while (g_hash_table_iter_next(&week_iter, &key, &value)) {
        GList *week_reservations = value;
        GList *top10 = get_top_n_passengers(week_reservations, 10);
        
        for (GList *t = top10; t != NULL; t = t->next) {
            PassengerSpending *ps = t->data;
            
            int *count = g_hash_table_lookup(top10_counts, ps->document_number);
            if (!count) {
                count = malloc(sizeof(int));
                *count = 0;
                g_hash_table_insert(top10_counts, strdup(ps->document_number), count);
            }
            (*count)++;
        }
        g_list_free_full(top10, free_passenger_spending);
    }
    
    g_hash_table_destroy(weeks);
    
    // Encontrar o melhor
    const char *best_passenger = NULL;
    int max_count = 0;
    
    g_hash_table_iter_init(&week_iter, top10_counts);
    while (g_hash_table_iter_next(&week_iter, &key, &value)) {
        const char *doc = key;
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
        } else {
            fprintf(out, "\n");
        }
    } else {
        fprintf(out, "\n");
    }
    
    g_hash_table_destroy(top10_counts);
}