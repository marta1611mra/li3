#define _XOPEN_SOURCE
#include "query5.h"
#include "flights_manager.h"
#include "flights.h"
#include "output_format.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <glib.h>

typedef struct {
    char airline_name[50];
    long total_delay_min;
    int num_delayed_flights;
} AirlineStatistics;

static int calculate_delay_min(const char *estim, const char *real) {
    if (!estim || !real || strcmp(real, "N/A") == 0) return 0;

    struct tm tm_estim = {0}, tm_real = {0};

    if (strptime(estim, "%Y-%m-%d %H:%M:%S", &tm_estim) == NULL) return 0;
    if (strptime(real, "%Y-%m-%d %H:%M:%S", &tm_real) == NULL) return 0;

    time_t t_estim = mktime(&tm_estim);
    time_t t_real = mktime(&tm_real);

    if(t_estim == -1 || t_real == -1) return 0;

    return (int)difftime(t_real, t_estim) / 60;
}

static int compare_stats(const void *a, const void *b) {
    const AirlineStatistics *s1 = (const AirlineStatistics *)a;
    const AirlineStatistics *s2 = (const AirlineStatistics *)b;

    double avg1 = (double)s1->total_delay_min / s1->num_delayed_flights;
    double avg2 = (double)s2->total_delay_min / s2->num_delayed_flights;

    if (avg2 > avg1) return 1;
    if (avg2 < avg1) return -1;

    return strcmp(s1->airline_name, s2->airline_name);
}

void query5(FlightsManager fm, int N, FILE *out) {
    GHashTable *tmp_statistics = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, g_free);
    GHashTable *flights_table = flights_manager_get_table(fm);
    GHashTableIter iter;
    gpointer value;
    
    g_hash_table_iter_init(&iter, flights_table);
    while (g_hash_table_iter_next(&iter, NULL, &value)) {
        Flight f = (Flight)value;
        if (get_flight_status(f) == Delayed) {
            const char *name = get_flight_airline(f);
            int delay = calculate_delay_min(get_flight_dep(f), get_flight_actual_dep(f));

            if (delay > 0) {
                AirlineStatistics *as = g_hash_table_lookup(tmp_statistics, name);
                if (!as) {
                    as = g_new0(AirlineStatistics, 1);
                    strncpy(as->airline_name, name, 49);
                    g_hash_table_insert(tmp_statistics, (gpointer)as->airline_name, as);
                }
                as->total_delay_min += delay;
                as->num_delayed_flights++;
            }
        }
    }
    GList *values = g_hash_table_get_values(tmp_statistics);
    int total_airlines = g_list_length(values);

    if (total_airlines == 0) {
        if (values) g_list_free(values);
        g_hash_table_destroy(tmp_statistics);
        return;
    }

    AirlineStatistics *sort_array = malloc(sizeof(AirlineStatistics) * total_airlines);
    int i = 0;
    for (GList *l = values; l != NULL; l = l->next) {
        sort_array[i++] = *(AirlineStatistics *)l->data;
    }

    qsort(sort_array, total_airlines, sizeof(AirlineStatistics), compare_stats);

    char sep = get_output_separator();

    for (int j = 0; j < N && j < total_airlines; j++) {
        double avg = (double)sort_array[j].total_delay_min / sort_array[j].num_delayed_flights;
        fprintf(out, "%s%c%d%c%.3f\n", 
                sort_array[j].airline_name, sep,
                sort_array[j].num_delayed_flights, sep,
                avg);
    }
    g_list_free(values);
    free(sort_array);
    g_hash_table_destroy(tmp_statistics);
}


