#include <glib.h>
#include <stdio.h>
#include <string.h>

#include "query4.h"
#include "dataset.h"
#include "passengers_manager.h"
#include "reservations_manager.h"
#include "passengers.h"
#include "reservations.h"

/*
Q4 determina qual o passageiro que esteve mais vezes no Top 10 semanal
Opcionalmente, a query pode receber um filtro que imponha o intervalo
de datas a considerar. Nesse caso apenas deverão ser tidos em conta os top
10 compreendidos entre esse intervalo de tempo
Considere ainda que uma semana tem início
no domingo e termina no sábado seguinte. Se o intervalo de tempo captar
uma semana de forma parcial, seja no início ou no fim, o top 10 dessa(s) se-
mana(s) também deve ser considerado. O campo que delimita as semanas
é dado pela data de partida estimada do voo (departure).
Em caso de empate, prevalece o passageiro com identificador (i.e.,
campo document number) mais baixo
*/

// Comando: 4 [begin_date end_date]
// Output: document_number;first_name;last_name;dob;nationality;count_top_10

typedef struct {
    char *document_number;
    double total_spent;
} PassengerSpending;

// Função de comparação para ordenar passageiros por gasto total (decrescente)
gint compare_passenger_spending(gconstpointer a, gconstpointer b) {
    const PassengerSpending *pa = a;
    const PassengerSpending *pb = b;
    if (pa->total_spent < pb->total_spent) return 1;
    if (pa->total_spent > pb->total_spent) return -1;
    // desempate por document_number
    return strcmp(pa->document_number, pb->document_number);
}


// Ajusta um intervalo de datas para semanas domingo a sábado.
void adjust_to_sunday_saturday(const GDate *begin, const GDate *end, GDate *begin_out, GDate *end_out) {
    g_return_if_fail(g_date_valid(begin));
    g_return_if_fail(g_date_valid(end));
    g_return_if_fail(begin_out != NULL);
    g_return_if_fail(end_out != NULL);

    // Copia as datas de entrada
    *begin_out = *begin;
    *end_out   = *end;

    // Ajustar begin para domingo 
    int begin_wd = g_date_get_weekday(begin_out);
    int days_to_subtract = begin_wd % 7;
    g_date_subtract_days(begin_out, days_to_subtract);


    // Ajustar end para sábado
    int end_wd = g_date_get_weekday(end_out);
    int days_to_add = (end_wd == 7) ? 6 : (6 - (end_wd - 1));
    g_date_add_days(end_out, days_to_add);
}


// Top n das reservas em geral num intervalo de datas
GList *top_reservations(GList *reservations, FlightsManager fm, int n) {
    // Criar hash table para somar gastos por passageiro, iterar reservas e somar gastos de cada passageiro
    GHashTable *spending_table = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);

    for (GList *l = reservations; l != NULL; l = l->next) {
        Reservation r = (Reservation)l->data;
        const char *doc = get_reservation_document_number(r);
        double price = get_reservation_total_price (r); 
        double *current_spent = g_hash_table_lookup(spending_table, doc);
            if (!current_spent) {
                current_spent = malloc(sizeof(double));
                *current_spent = 0.0;
            }        
        *current_spent += price;
        g_hash_table_insert(spending_table, strdup(doc), current_spent);
    }

    // Criar uma lista com os passageiros e seus gastos totais
    GList *passenger_list = NULL;
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, spending_table);

    while (g_hash_table_iter_next(&iter, &key, &value)) {
        PassengerSpending *ps = malloc(sizeof(PassengerSpending));
        ps->document_number = strdup((char *)key);  // cópia da string
        ps->total_spent = *(double *)value;         // copiar valor do total gasto
        passenger_list = g_list_prepend(passenger_list, ps);
    }

    // Ordenar passageiros por total gasto decrescente
    passenger_list = g_list_sort(passenger_list, compare_passenger_spending);

    // Selecionar top N
    GList *top_n = NULL;
    int count = 0;
    for (GList *l = passenger_list; l != NULL && count < n; l = l->next, count++) {
        top_n = g_list_append(top_n, l->data);
    }

    g_list_free(passenger_list);
    g_hash_table_destroy(spending_table);

    return top_n;  
}

// Executa a query 4
void query4_execute(Dataset d, const char *begin_date, const char *end_date) {
    ReservationsManager rm = dataset_get_reservations(d);
    FlightsManager fm = dataset_get_flights(d);
    GList *all_reservations = reservations_manager_get_all(rm);

    // Hash table: chave = domingo da semana, valor = lista de reservas dessa semana
    GHashTable *weeks_table = g_hash_table_new_full(g_str_hash, g_str_equal, free, (GDestroyNotify)g_list_free);

    GDate week_start, week_end;
    if (begin_date && end_date) {
        GDate begin, end;
        g_date_clear(&begin, TRUE);
        g_date_clear(&end, TRUE);
        g_date_set_parse(&begin, begin_date);
        g_date_set_parse(&end, end_date);
        adjust_to_sunday_saturday(&begin, &end, &week_start, &week_end);
    }

    // Agrupar reservas por semana
    for (GList *l = all_reservations; l != NULL; l = l->next) {
        Reservation r = (Reservation)l->data;

        Flight f = flights_manager_get(fm, get_reservation_flight_id(r, 0));
        if (!f) continue;

        const char *departure = get_flight_dep(f);

        GDate week_dep;
        g_date_clear(&week_dep, TRUE);
        g_date_set_parse(&week_dep, departure);
        if (!g_date_valid(&week_dep)) continue;

        // Calcular o domingo da semana
        int wd = g_date_get_weekday(&week_dep);
        g_date_subtract_days(&week_dep, wd % 7);

        // Filtrar por intervalo de datas
        if (begin_date && end_date) {
            if (g_date_compare(&week_dep, &week_start) < 0 ||
                g_date_compare(&week_dep, &week_end) > 0) {
                continue; // semana fora do intervalo
            } 
        }

        char week_str[11];
        g_date_strftime(week_str, sizeof(week_str), "%Y-%m-%d", &week_dep);

        char *key = g_strdup(week_str);

        GList *week_res = g_hash_table_lookup(weeks_table, week_str);
        week_res = g_list_prepend(week_res, r);
        g_hash_table_insert(weeks_table, key, week_res);
    }

    // Contar presenças no top 10./.
    GHashTable *passenger_count = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, weeks_table);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
    GList *week_reservations = (GList *)value;
    GList *top_10 = top_reservations(week_reservations, fm, 10);

    for (GList *t = top_10; t != NULL; t = t->next) {
        PassengerSpending *ps = (PassengerSpending *)t->data;
        int *count = g_hash_table_lookup(passenger_count, ps->document_number);
        if (!count) {
            count = malloc(sizeof(int));
            *count = 0;
            g_hash_table_insert(passenger_count, strdup(ps->document_number), count);
        }
        (*count)++;
    }

    for (GList *t = top_10; t != NULL; t = t->next) {
        PassengerSpending *ps = (PassengerSpending *)t->data;
        free(ps->document_number);
        free(ps);
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
        if (*count > max_count || (*count == max_count && (top_passenger == NULL || strcmp(doc, top_passenger) < 0))) {
            max_count = *count;
            top_passenger = doc;
        }
    }
    
    FILE *output = fopen("resultados/command4_output.txt", "w");
    // Output do resultado
    if (top_passenger) {
        PassengersManager pm = dataset_get_passengers(d);
        Passenger p = passengers_manager_get(pm, top_passenger);
        fprintf(output, "%s;%s;%s;%s;%s;%d\n",
               top_passenger,
               get_passenger_first_name(p),
               get_passenger_last_name(p),
               get_passenger_dob(p),
               get_passenger_nationality(p),
               max_count);
    } else {
        fprintf(output, "\n");
         }
    fclose(output);

    g_hash_table_destroy(weeks_table);
    g_hash_table_destroy(passenger_count);
}
