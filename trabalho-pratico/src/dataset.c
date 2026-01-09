#define _XOPEN_SOURCE 700
#include "dataset.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <glib.h>


/* ===== Estrutura para Query 5 (Estatísticas de Atraso) ===== */
typedef struct {
    char *airline_name;
    long total_delay;
    int count;
    double average; // Calculado no final para ordenação rápida
} Q5Stats;


/* ===== Estrutura interna ===== */

struct dataset {
    AirportsManager airports;
    AircraftsManager aircrafts;
    FlightsManager flights;
    PassengersManager passengers;
    ReservationsManager reservations;

    GHashTable *q2_index;   // manufacturer -> (aircraft_id -> count) 
    GHashTable *q3_index;   // airport -> (date -> count) 
    GHashTable *q4_weeks;   // week_sunday -> (document -> total_price) 
    GHashTable *q6_index;   // nationality -> (airport -> count) 
    GHashTable *q5_temp_map; // Temporário durante o parsing
    Q5Stats *q5_array;       // Array final ordenado
    int q5_count;            // Número de companhias
};

static void free_q5_stat(void *data) {
    Q5Stats *stat = (Q5Stats *)data;
    if (stat) {
        free(stat->airline_name);
    }
    free(stat);
}
// Calcula domingo da semana, assumindo date no formato YYYY-MM-DD
static void calculate_week_sunday(const char *date, char out[11]) {
    int y, m, d;
    // Lê a data YYYY-MM-DD
    if (sscanf(date, "%d-%d-%d", &y, &m, &d) != 3) {
        return;
    }

    // Usa GDate para evitar problemas de fuso horário do mktime
    GDate gdate;
    g_date_clear(&gdate, 1);
    g_date_set_dmy(&gdate, d, m, y);

    if (!g_date_valid(&gdate)) return;

    // Obtém dia da semana (Domingo=7, Segunda=1, etc.)
    GDateWeekday wd = g_date_get_weekday(&gdate);

    // Calcula quantos dias recuar até Domingo
    // Se for Domingo (7): 7 % 7 = 0 dias
    // Se for Segunda (1): 1 % 7 = 1 dia
    int days_to_subtract = wd % 7; 

    g_date_subtract_days(&gdate, days_to_subtract);

    // Escreve o resultado
    g_date_strftime(out, 11, "%Y-%m-%d", &gdate);
}

static int fast_delay_calc(const char *schedule, const char *real) {
    if (!schedule || !real || !schedule) return 0;

    struct tm tm_sch = {0}, tm_real = {0};
    int y=0, M=0, d=0, h=0, m=0; 

    if (sscanf(schedule, "%d-%d-%d %d:%d", &y, &M, &d, &h, &m) < 5) return 0;
    tm_sch.tm_year = y - 1900; tm_sch.tm_mon = M - 1; tm_sch.tm_mday = d;
    tm_sch.tm_hour = h; tm_sch.tm_min = m; tm_sch.tm_sec = 0; tm_sch.tm_isdst = -1;

    y=0; M=0; d=0; h=0; m=0; 

    if (sscanf(real, "%d-%d-%d %d:%d", &y, &M, &d, &h, &m) < 5) return 0;
    tm_real.tm_year = y - 1900; tm_real.tm_mon = M - 1; tm_real.tm_mday = d;
    tm_real.tm_hour = h; tm_real.tm_min = m; tm_real.tm_sec = 0; tm_real.tm_isdst = -1;

    time_t t_sch = mktime(&tm_sch);
    time_t t_real = mktime(&tm_real);

    if (t_sch == -1 || t_real == -1) return 0;

    return (int)(difftime(t_real, t_sch) / 60.0);
}

// Comparador para qsort da Query 5
static int compare_q5(const void *a, const void *b) {
    const Q5Stats *s1 = (const Q5Stats *)a;
    const Q5Stats *s2 = (const Q5Stats *)b;

    if (s2->average > s1->average) return 1;
    if (s2->average < s1->average) return -1;
    return strcmp(s1->airline_name, s2->airline_name);
}


// Funções de criação/destruição do Dataset
Dataset dataset_create(void) {
    Dataset d = malloc(sizeof(struct dataset));
    if (!d) return NULL;

    d->airports     = airports_manager_create();
    d->aircrafts    = aircrafts_manager_create();
    d->flights      = flights_manager_create();
    d->passengers   = passengers_manager_create();
    d->reservations = reservations_manager_create();

    d->q2_index = g_hash_table_new_full(
        g_str_hash, g_str_equal, g_free,
        (GDestroyNotify) g_hash_table_destroy
    );

    d->q3_index = g_hash_table_new_full(
        g_str_hash, g_str_equal, g_free,
        (GDestroyNotify) g_hash_table_destroy
    );

    d->q4_weeks = g_hash_table_new_full(
        g_str_hash, g_str_equal, g_free,
        (GDestroyNotify) g_hash_table_destroy
    );

    d->q6_index = g_hash_table_new_full(
        g_str_hash, g_str_equal, g_free,
        (GDestroyNotify) g_hash_table_destroy
    );

    d->q5_temp_map = g_hash_table_new_full(
        g_str_hash, g_str_equal, NULL, (GDestroyNotify)free_q5_stat);
    d->q5_array = NULL;
    d->q5_count = 0;

    return d;
}

void dataset_destroy(Dataset d) {
    if (!d) return;

    g_hash_table_destroy(d->q2_index);
    g_hash_table_destroy(d->q3_index);
    g_hash_table_destroy(d->q4_weeks);
    g_hash_table_destroy(d->q6_index);
    if (d->q5_temp_map) g_hash_table_destroy(d->q5_temp_map);
    if (d->q5_array) {
        for (int i=0; i < d->q5_count; i++) {
        free(d->q5_array[i].airline_name);
        }
        free(d->q5_array);
    }

    airports_manager_destroy(d->airports);
    aircrafts_manager_destroy(d->aircrafts);
    flights_manager_destroy(d->flights);
    passengers_manager_destroy(d->passengers);
    reservations_manager_destroy(d->reservations);

    free(d);
}

// Getters para os managers e índices

AirportsManager dataset_get_airports(Dataset d) { return d->airports; }
AircraftsManager dataset_get_aircrafts(Dataset d) { return d->aircrafts; }
FlightsManager dataset_get_flights(Dataset d) { return d->flights; }
PassengersManager dataset_get_passengers(Dataset d) { return d->passengers; }
ReservationsManager dataset_get_reservations(Dataset d) { return d->reservations; }

GHashTable *dataset_get_q2_index(Dataset d) { return d->q2_index; }
GHashTable *dataset_get_q3_index(Dataset d) { return d->q3_index; }
GHashTable *dataset_get_q4_weeks(Dataset d) { return d->q4_weeks; }
GHashTable *dataset_get_q6_index(Dataset d) { return d->q6_index; }


// Query 2
static void q2_update_inner(GHashTable *index,
                            const char *key,
                            const char *aircraft_id) {
    GHashTable *inner = g_hash_table_lookup(index, key);

    if (!inner) {
        inner = g_hash_table_new_full(
            g_str_hash, g_str_equal, g_free, g_free);
        g_hash_table_insert(index, g_strdup(key), inner);
    }

    int *count = g_hash_table_lookup(inner, aircraft_id);
    if (!count) {
        count = malloc(sizeof(int));
        *count = 1;
        g_hash_table_insert(inner, g_strdup(aircraft_id), count);
    } else {
        (*count)++;
    }
}

void dataset_update_q2(Dataset d,
                       const char *aircraft_id,
                       const char *manufacturer) {
    if (!d || !aircraft_id || !manufacturer) return;

    q2_update_inner(d->q2_index, manufacturer, aircraft_id);
    q2_update_inner(d->q2_index, "__ALL__", aircraft_id);
}


// Query 3
void dataset_update_q3(Dataset d,
                       const char *airport_code,
                       const char *date) {
    if (!d || !airport_code || !date) return;

    char date_only[11];
    strncpy(date_only, date, 10);
    date_only[10] = '\0';

    GHashTable *date_table =
        g_hash_table_lookup(d->q3_index, airport_code);

    if (!date_table) {
        date_table = g_hash_table_new_full(
            g_str_hash, g_str_equal, g_free, NULL);
        g_hash_table_insert(d->q3_index,
                            g_strdup(airport_code),
                            date_table);
    }

    gpointer val = g_hash_table_lookup(date_table, date_only);
    int count = val ? GPOINTER_TO_INT(val) + 1 : 1;

    g_hash_table_insert(date_table,
                        g_strdup(date_only),
                        GINT_TO_POINTER(count));
}


// Query 4
void dataset_add_q4_data(Dataset d,
                         const char *document,
                         double price,
                         const char *date) {
    if (!d || !document || !date) return;

    char week[11];
    calculate_week_sunday(date, week);

    GHashTable *week_table =
        g_hash_table_lookup(d->q4_weeks, week);

    if (!week_table) {
        week_table = g_hash_table_new_full(
            g_str_hash, g_str_equal, g_free, g_free);
        g_hash_table_insert(d->q4_weeks,
                            g_strdup(week),
                            week_table);
    }

    double *total = g_hash_table_lookup(week_table, document);
    if (!total) {
        total = malloc(sizeof(double));
        *total = price;
        g_hash_table_insert(week_table,
                            g_strdup(document),
                            total);
    } else {
        *total += price;
    }
}


// Query 6
void dataset_update_q6(Dataset d,
                       const char *nationality,
                       const char *airport) {
    if (!d || !nationality || !airport) return;

    GHashTable *nat_table =
        g_hash_table_lookup(d->q6_index, nationality);

    if (!nat_table) {
        nat_table = g_hash_table_new_full(
            g_str_hash, g_str_equal, g_free, g_free);
        g_hash_table_insert(d->q6_index,
                            g_strdup(nationality),
                            nat_table);
    }

    int *count = g_hash_table_lookup(nat_table, airport);
    if (!count) {
        count = malloc(sizeof(int));
        *count = 1;
        g_hash_table_insert(nat_table,
                            g_strdup(airport),
                            count);
    } else {
        (*count)++;
    }
}

void dataset_build_q6_index(Dataset d) {
    (void)d; 
}

// Query 5
void dataset_update_q5(Dataset d, char *airline, char *sched_dep, char *real_dep) {
    int delay = fast_delay_calc(sched_dep, real_dep);
    if (delay <= 0) return;

    Q5Stats *stat = g_hash_table_lookup(d->q5_temp_map, airline);
    if (!stat) {
        stat = malloc(sizeof(Q5Stats));
        stat->airline_name = strdup(airline);
        stat->total_delay = 0;
        stat->count = 0;
        g_hash_table_insert(d->q5_temp_map, stat->airline_name, stat);
    }
    stat->total_delay += delay;
    stat->count++;
}

void dataset_finalize_q5(Dataset d) {
    if (!d || !d->q5_temp_map) return;

    d->q5_count = g_hash_table_size(d->q5_temp_map);
    d->q5_array = malloc(sizeof(Q5Stats) * d->q5_count);

    GHashTableIter iter;
    gpointer key, value;
    int i = 0;

    g_hash_table_iter_init(&iter, d->q5_temp_map);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        Q5Stats *stat = (Q5Stats *)value;
        // Calcula a média antes de passar para o array
        if (stat->count > 0) {
            stat->average = (double)stat->total_delay / stat->count;
            stat->average = round(stat->average * 1000.0) / 1000.0;
        } else {
            stat->average = 0;
        }

        d->q5_array[i].airline_name = strdup(stat->airline_name);
        d->q5_array[i].total_delay = stat->total_delay;
        d->q5_array[i].count = stat->count;
        d->q5_array[i].average = stat->average;
        i++;
    }

    // Limpeza da tabela temporária
    g_hash_table_destroy(d->q5_temp_map);
    d->q5_temp_map = NULL;

    // Ordenação final
    qsort(d->q5_array, d->q5_count, sizeof(Q5Stats), compare_q5);
}

int dataset_q5_get_count(Dataset d) {
    return d ? d->q5_count : 0;
}

// Retorna o nome da companhia na posição 'index'
char *dataset_q5_get_name(Dataset d, int index) {
    if (!d || !d->q5_array || index < 0 || index >= d->q5_count) return NULL;
    return d->q5_array[index].airline_name;
}

// Retorna o nº de voos na posição 'index'
int dataset_q5_get_flights_count(Dataset d, int index) {
    if (!d || !d->q5_array || index < 0 || index >= d->q5_count) return 0;
    return d->q5_array[index].count;
}

// Retorna a média na posição 'index'
double dataset_q5_get_average(Dataset d, int index) {
    if (!d || !d->q5_array || index < 0 || index >= d->q5_count) return 0.0;
    return d->q5_array[index].average;
}