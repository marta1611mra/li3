#include <glib.h>
#include "query4.h"
#include "dataset.h"
#include "passengers_manager.h"
#include "reservations_manager.h"
#include "passengers.h"
#include "reservations.h"
#include <stdio.h>
#include <string.h>

//Calcula qual passageiro esteve mais vezes no Top 10 de passageiros
// que mais gastaram semanalmente durante o intervalo [start, end].
// Output: document_number;first_name;last_name;dob;nationality;count_top_10


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

void query4_execute(Dataset d, const char *start, const char *end) {
    PassengersManager pm = dataset_get_passengers(d);
    ReservationsManager rm = dataset_get_reservations(d);

    GHashTable *number_weeks_top = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);

    // Iterar por semanas no intervalo [start, end]
    char current_week[11];
    strcpy(current_week, start);

    while (strcmp(current_week, end) <= 0) {
        GHashTable *week_totals = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);

        // Iterar por todas as reservas
        GList *reservations = reservations_manager_get_all(rm);
        for (GList *node = reservations; node != NULL; node = node->next) {
            Reservation r = (Reservation)node->data;

            // Verificar se a reserva pertence à semana atual
            const char *flight_id = get_reservation_flight_id(r, 0);
            if (flight_id && strncmp(flight_id, current_week, 10) == 0) {
                const char *passenger_id = get_reservation_document_number(r);
                double price = get_reservation_price(r, 0);
                calculate_week_totals(week_totals, passenger_id, price);
            }
        }
        g_list_free(reservations);

        // Atualizar top 10 da semana
        update_top10(week_totals, number_weeks_top);

        g_hash_table_destroy(week_totals);

        // Avançar para a próxima semana (simples incremento para exemplo)
        // Em produção, deve-se usar uma função adequada para manipular datas.
        int year, week;
        sscanf(current_week, "%4d-W%2d", &year, &week);
        week++;
        if (week > 52) {
            week = 1;
            year++;
        }
        sprintf(current_week, "%04d-W%02d", year, week);
    }

    // Encontrar passageiro com mais semanas no top 10
    GHashTableIter iter;
    gpointer key, value;
    const char *top_passenger_id = NULL;
    int max_count = 0;

    g_hash_table_iter_init(&iter, number_weeks_top);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        int count = *((int *)value);
        if (count > max_count) {
            max_count = count;
            top_passenger_id = (const char *)key;
        }
    }   
    if (top_passenger_id) {
        Passenger p = passengers_manager_get(pm, top_passenger_id);
        if (p) {
            printf("%s;%s;%s;%s;%s;%d\n",
                   get_passenger_document_id(p),
                   get_passenger_first_name(p),
                   get_passenger_last_name(p),
                   get_passenger_dob(p),
                   get_passenger_nationality(p),
                   max_count);
        }
    }
    g_hash_table_destroy(number_weeks_top);
}
