#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include "dataset.h"
#include "reservations_manager.h"
#include "passengers_manager.h"
#include "date.h"
#include "query4.h"

// Estrutura auxiliar com o id e o total gasto na semana do passageiro
typedef struct {
    char *passenger_id;
    double week_spendings;  
} Week_Total;

// Comparador para usar no qsort: descendente por gasto, ascendente por id em caso de empate
static int compare_week_totals(const void *a, const void *b) {
    const Week_Total *A = a;
    const Week_Total *B = b;
    if (A->week_spendings < B->week_spendings) return 1;
    if (A->week_spendings > B->week_spendings) return -1;
    return strcmp(A->passenger_id, B->passenger_id);
}

// Acumula gasto semanal de um passageiro na hashtable week_totals
static void calculate_week_totals(GHashTable *week_totals, const char *id, double price) {
    if (!id) return;
    double *old_total = g_hash_table_lookup(week_totals, id);
    if (!old_total) {
        double *v = g_malloc(sizeof(double));
        *v = price;
        g_hash_table_insert(week_totals, g_strdup(id), v);
    } else {
        *old_total += price;
    }
}

// Atualiza contador do número de semanas no Top10 do passageiro
static void update_top10(GHashTable *week_totals, GHashTable *number_weeks_top) {
    if (!week_totals || g_hash_table_size(week_totals) == 0) return;

    int n = g_hash_table_size(week_totals);
    Week_Total *array = g_malloc(sizeof(Week_Total) * n);

    GHashTableIter iter;
    gpointer key, value;
    int i = 0;
    g_hash_table_iter_init(&iter, week_totals);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        array[i].passenger_id = (char *)key;
        array[i].week_spendings = *((double *)value);
        i++;
    }

    qsort(array, n, sizeof(Week_Total), compare_week_totals);

    int limit = (n < 10 ? n : 10);
    for (int k = 0; k < limit; k++) {
        char *id = array[k].passenger_id;
        int *count = g_hash_table_lookup(number_weeks_top, id);
        if (!count) {
            int *new_count = g_malloc(sizeof(int));
            *new_count = 1;
            g_hash_table_insert(number_weeks_top, g_strdup(id), new_count);
        } else {
            (*count)++;
        }
    }

    g_free(array);
}

// Função principal da query4
void query4(Dataset dataset, Date start, Date end, FILE *out) {
    if (!dataset || !out) return;

    ReservationsManager rm = dataset_get_reservations(dataset);
    PassengersManager pm = dataset_get_passengers(dataset);

    GHashTable *number_weeks_top = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    GPtrArray *all_reservations = reservations_manager_sorted_by_date(rm);

    if (!all_reservations || all_reservations->len == 0) {
        fprintf(out, "\n");
        if (all_reservations) g_ptr_array_free(all_reservations, TRUE);
        g_hash_table_destroy(number_weeks_top);
        return;
    }

    Date current = week_start(start);
    Date last = week_end(end);

    while (date_compare(current, last) <= 0) {
        Date s = current;
        Date e = date_add_days(s, 6);

        GHashTable *week_totals = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);

        for (guint i = 0; i < all_reservations->len; i++) {
            Reservation r = g_ptr_array_index(all_reservations, i);
            Date rd = reservation_get_date(r);
            if (date_in_interval(rd, s, e)) {
                calculate_week_totals(week_totals,
                                      reservation_get_passenger_id(r),
                                      reservation_get_price(r));
            }
        }

        update_top10(week_totals, number_weeks_top);
        g_hash_table_destroy(week_totals);

        current = date_add_days(current, 7);
    }

    // Determinar o passageiro vencedor
    char *best_id = NULL;
    int best_count = -1;
    GHashTableIter it;
    gpointer k, v;
    g_hash_table_iter_init(&it, number_weeks_top);
    while (g_hash_table_iter_next(&it, &k, &v)) {
        char *id = (char *)k;
        int c = *((int *)v);
        if (!best_id || c > best_count || (c == best_count && strcmp(id, best_id) < 0)) {
            best_id = id;
            best_count = c;
        }
    }

    if (!best_id) {
        fprintf(out, "\n");
    } else {
        Passenger p = passengers_manager_get(pm, best_id);
        if (!p) {
            fprintf(out, "%s;%d\n", best_id, best_count);
        } else {
            fprintf(out, "%s;%s;%s;%s;%s;%d\n",
                    passenger_get_id(p),
                    passenger_get_first(p),
                    passenger_get_last(p),
                    passenger_get_dob(p),
                    passenger_get_nat(p),
                    best_count);
        }
    }

    g_ptr_array_free(all_reservations, TRUE);
    g_hash_table_destroy(number_weeks_top);
}
