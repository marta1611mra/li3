#include <glib.h>
#include "query4.h"
#include "dataset.h"
#include "passengers_manager.h"
#include "reservations_manager.h"
#include "passengers.h"
#include "reservations.h"
#include <stdio.h>
#include <string.h>

// Estrutura auxiliar para gasto semanal
typedef struct {
    const char *passenger_id;
    double week_spendings;
} Week_Total;

static int compare_week_totals(const void *a, const void *b) {
    const Week_Total *A = a;
    const Week_Total *B = b;
    if (A->week_spendings < B->week_spendings) return 1;
    if (A->week_spendings > B->week_spendings) return -1;
    return strcmp(A->passenger_id, B->passenger_id);
}

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

static void update_top10(GHashTable *week_totals, GHashTable *number_weeks_top) {
    if (!week_totals || g_hash_table_size(week_totals) == 0) return;

    int n = g_hash_table_size(week_totals);
    Week_Total *array = g_malloc(sizeof(Week_Total) * n);

    GHashTableIter iter;
    gpointer key, value;
    int i = 0;
    g_hash_table_iter_init(&iter, week_totals);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        array[i].passenger_id = (const char *)key;
        array[i].week_spendings = *((double *)value);
        i++;
    }

    qsort(array, n, sizeof(Week_Total), compare_week_totals);

    int limit = (n < 10 ? n : 10);
    for (int k = 0; k < limit; k++) {
        const char *id = array[k].passenger_id;
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