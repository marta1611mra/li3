#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "query4.h"
#include "dataset.h"
#include "passengers_manager.h"
#include "reservations_manager.h"
#include "passengers.h"
#include "reservations.h"

// Estrutura auxiliar para pares chave/valor
typedef struct {
    gpointer key;    // document_number
    gpointer value;  // double* gasto total
} KeyValuePair;

// Função de comparação para ordenar passageiros por gasto total (decrescente)
gint compare_passenger_spending(gconstpointer a, gconstpointer b) {
    const KeyValuePair *pa = a;
    const KeyValuePair *pb = b;
    double da = *(double *)pa->value;
    double db = *(double *)pb->value;
    if (da < db) return 1;   // decrescente
    else if (da > db) return -1;
    return 0;
}

// Calcula o domingo (início da semana) e sábado (fim da semana) de um intervalo
void adjust_to_sunday_saturday(const GDate *begin, const GDate *end, GDate *begin_out, GDate *end_out) {
    g_return_if_fail(g_date_valid(begin));
    g_return_if_fail(g_date_valid(end));
    g_return_if_fail(begin_out != NULL);
    g_return_if_fail(end_out != NULL);

    *begin_out = *begin;
    *end_out   = *end;

    // Domingo anterior ou igual
    int begin_wd = g_date_get_weekday(begin_out);
    int days_to_subtract = (begin_wd == 7) ? 0 : begin_wd;
    g_date_subtract_days(begin_out, days_to_subtract);

    // Próximo sábado ou igual
    int end_wd = g_date_get_weekday(end_out);
    int days_to_add = (end_wd == 7) ? 6 : (6 - (end_wd - 1));
    g_date_add_days(end_out, days_to_add);
}

// Calcula top N passageiros por gasto em uma lista de reservas
GList *top_reservations(GList *reservations, int n) {
    // Agrupar por passenger_id e somar gastos
    GHashTable *passengers = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);
    for (GList *iter = reservations; iter != NULL; iter = iter->next) {
        Reservation r = (Reservation)iter->data;
        const char *pid = get_reservation_document_number(r);
        double price = get_reservation_price(r);

        double *total_spent = g_hash_table_lookup(passengers, pid);
        if (!total_spent) {
            total_spent = malloc(sizeof(double));
            *total_spent = 0.0;
            g_hash_table_insert(passengers, strdup(pid), total_spent);
        }
        *total_spent += price;
    }

    // Converter hash table em lista de KeyValuePair*
    GList *list = NULL;
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, passengers);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        KeyValuePair *pair = g_new(KeyValuePair, 1);
        pair->key = key;
        pair->value = value;
        list = g_list_prepend(list, pair);
    }

    // Ordenar decrescentemente por gasto
    list = g_list_sort(list, compare_passenger_spending);

    // Selecionar top N
    GList *top_n = NULL;
    int count = 0;
    for (GList *l = list; l != NULL && count < n; l = l->next, count++) {
        top_n = g_list_prepend(top_n, l->data);
    }
    g_list_free(list);
    g_hash_table_destroy(passengers);

    return top_n;
}

// Função principal
void query4_execute(Dataset d, const char *begin_date, const char *end_date) {
    ReservationsManager rm = dataset_get_reservations_manager(d);
    GList *all_reservations = reservations_manager_get_all(rm);
    FlightsManager fm = dataset_get_flights_manager(d);

    // Hash table: chave = domingo da semana (string), valor = lista de reservas
    GHashTable *dates = g_hash_table_new_full(g_str_hash, g_str_equal, free, (GDestroyNotify)g_list_free);

    // Ajuste de filtro de datas
    GDate filter_start, filter_end;
    gboolean filter = FALSE;
    if (begin_date && end_date) {
        GDate begin, end;
        if (!g_date_set_parse(&begin, begin_date) || !g_date_set_parse(&end, end_date)) {
            fprintf(stderr, "Data inválida!\n");
            return;
        }
        adjust_to_sunday_saturday(&begin, &end, &filter_start, &filter_end);
        filter = TRUE;
    }

    // Organizar reservas por semana (domingo a sábado)
    for (GList *l = all_reservations; l != NULL; l = l->next) {
        Reservation r = (Reservation)l->data;
        const char *flight_id = get_reservation_flight_id(r);
        Flight f = flights_manager_get(fm, flight_id);
        const GDate *departure_date = get_flight_dep(f);

        // Filtro opcional de datas
        if (filter && (g_date_compare(departure_date, &filter_start) < 0 ||
                       g_date_compare(departure_date, &filter_end) > 0)) {
            continue;
        }

        // Domingo da semana do voo
        GDate week_start;
        week_start = *departure_date;
        int wd = g_date_get_weekday(&week_start);
        g_date_subtract_days(&week_start, (wd == 7) ? 0 : wd);

        char *week_key = g_strdup(g_date_to_string(&week_start));

        GList *week_reservations = g_hash_table_lookup(dates, week_key);
        week_reservations = g_list_prepend(week_reservations, r);
        g_hash_table_insert(dates, week_key, week_reservations);
    }

    // Contagem de aparições no top 10
    GHashTable *passenger_count = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, dates);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        GList *week_reservations = (GList *)value;
        GList *top_10 = top_reservations(week_reservations, 10);

        for (GList *t = top_10; t != NULL; t = t->next) {
            KeyValuePair *pair = (KeyValuePair *)t->data;
            const char *doc = (const char *)pair->key;

            int *count = g_hash_table_lookup(passenger_count, doc);
            if (!count) {
                count = malloc(sizeof(int));
                *count = 0;
                g_hash_table_insert(passenger_count, strdup(doc), count);
            }
            (*count)++;
        }

        // Liberar memória do top 10
        for (GList *tmp = top_10; tmp != NULL; tmp = tmp->next) {
            KeyValuePair *pair = tmp->data;
            g_free(pair); 
        }
        g_list_free(top_10);
    }

    // Encontrar passageiro com maior contagem
    int max_count = -1;
    const char *top_passenger = NULL;
    g_hash_table_iter_init(&iter, passenger_count);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        const char *doc = (const char *)key;
        int *count = (int *)value;
        if (*count > max_count || (*count == max_count && strcmp(doc, top_passenger) < 0)) {
            max_count = *count;
            top_passenger = doc;
        }
    }

    // Imprimir resultado
    if (top_passenger) {
        PassengersManager pm = dataset_get_passengers_manager(d);
        Passenger p = passengers_manager_get(pm, top_passenger);
        printf("%s;%s;%s;%s;%s;%d\n",
               top_passenger,
               get_passenger_first_name(p),
               get_passenger_last_name(p),
               get_passenger_dob(p),
               get_passenger_nationality(p),
               max_count);
    }

    // Limpar memória
    g_hash_table_destroy(dates);
    g_hash_table_destroy(passenger_count);
}
