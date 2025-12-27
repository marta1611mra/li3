#include "parser/parser.h"
#include "dataset.h"
#include "syntactic_validation.h"
#include "logical_validation.h"
#include "reservations.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <glib.h>

// Lê e processa o ficheiro reservations.csv.
void parse_reservations(Dataset d, const char *data_path) {
    char clean_path[512];
    strcpy(clean_path, data_path);
    size_t len = strlen(clean_path);
    while (len > 0 && clean_path[len - 1] == '/') {
        clean_path[len - 1] = '\0';
        len--;
    }

    char path[1026];
    snprintf(path, sizeof(path), "%s/reservations.csv", clean_path);

    FILE *f = fopen(path, "r");
    if (!f) { perror(path); return; }

    exist_result();
    FILE *ferror = fopen("resultados/reservations_errors.csv", "w");
    if (!ferror) { perror("reservations_errors.csv"); fclose(f); return; }

    wcsv_header(f, ferror);

    char line[2048];
    while (fgets(line, sizeof(line), f)) {

        char reservation_id[21] = "";
        char flight_list[128] = "";
        char document_number[16] = "";
        char seat_str[8] = "";
        char extra_luggage_str[8] = "";
        char priority_boarding_str[8] = "";
        char qr_code[4096] = "";
        double price = 0.0;

        int n = sscanf(line,
            "\"%20[^\"]\",\"%127[^\"]\",\"%15[^\"]\",\"%7[^\"]\",\"%lf\",\"%7[^\"]\",\"%7[^\"]\",\"%4095[^\"]\"",
            reservation_id, flight_list, document_number, seat_str,
            &price, extra_luggage_str, priority_boarding_str, qr_code);

        if (n != 8) {
            fprintf(ferror, "%s", line);
            continue;
        }

        remove_quotes(reservation_id); remove_spc(reservation_id);
        remove_quotes(flight_list); remove_spc(flight_list);
        remove_quotes(document_number); remove_spc(document_number);
        remove_quotes(extra_luggage_str); remove_spc(extra_luggage_str);
        remove_quotes(priority_boarding_str); remove_spc(priority_boarding_str);
        remove_quotes(qr_code); remove_spc(qr_code);

        reservation_id[strcspn(reservation_id, "\r\n")] = 0;
        flight_list[strcspn(flight_list, "\r\n")] = 0;
        document_number[strcspn(document_number, "\r\n")] = 0;
        extra_luggage_str[strcspn(extra_luggage_str, "\r\n")] = 0;
        priority_boarding_str[strcspn(priority_boarding_str, "\r\n")] = 0;
        qr_code[strcspn(qr_code, "\r\n")] = 0;

        if (!validate_reservation_id(reservation_id) ||
            !validate_document_number(document_number) ||
            !validate_csv_lists(flight_list)) {
            fprintf(ferror, "%s", line);
            continue;
        }

        if (!passengers_manager_exists(dataset_get_passengers(d), document_number)) {
            fprintf(ferror, "%s", line);
            continue;
        }

        /* ===== Extrair flight IDs ===== */
        char flight_id[2][10] = {{0}};
        int num_ids = 0;

        char list_copy[128];
        strncpy(list_copy, flight_list, sizeof(list_copy));
        list_copy[sizeof(list_copy) - 1] = '\0';

        /* remover [ ] */
        char *start = strchr(list_copy, '[');
        char *end   = strrchr(list_copy, ']');

        if (!start || !end || start >= end) {
            fprintf(ferror, "%s", line);
            continue;
        }

        *end = '\0';
        start++;

        /* agora start contém: 'FL123','FL456' */
        char *token = strtok(start, ",");

        while (token && num_ids < 2) {
            while (*token == ' ' || *token == '\'') token++;

            char *quote = strchr(token, '\'');
            if (quote) *quote = '\0';

            strncpy(flight_id[num_ids], token, 9);
            flight_id[num_ids][9] = '\0';
            num_ids++;

            token = strtok(NULL, ",");
        }

        if (num_ids == 0) {
            fprintf(ferror, "%s", line);
            continue;
        }

        FlightsManager fm = dataset_get_flights(d);
        for (int i = 0; i < num_ids; i++) {
            if (!validate_flight_id(flight_id[i]) ||
                !flights_manager_get(fm, flight_id[i])) {
                fprintf(ferror, "%s", line);
                goto next_line;
            }
        }

        if (num_ids == 2) {
            Flight f1 = flights_manager_get(fm, flight_id[0]);
            Flight f2 = flights_manager_get(fm, flight_id[1]);
            if (strcmp(get_flight_dest(f1), get_flight_orig(f2)) != 0) {
                fprintf(ferror, "%s", line);
                continue;
            }
        }

        int extra_luggage = (strcmp(extra_luggage_str, "true") == 0);
        int priority_boarding = (strcmp(priority_boarding_str, "true") == 0);

        /* Se o ID já existir, o add irá substituir a reserva antiga */
        ReservationsManager rm = dataset_get_reservations(d);

        int *seats = malloc(2 * sizeof(int));
        double *prices = malloc(2 * sizeof(double));
        int *luggage = malloc(2 * sizeof(int));
        int *priority = malloc(2 * sizeof(int));

        if (!seats || !prices || !luggage || !priority) {
            free(seats); free(prices); free(luggage); free(priority);
            fprintf(ferror, "%s", line);
            continue;
        }

        seats[0] = 0; seats[1] = 0;
        prices[0] = price; prices[1] = 0.0;
        luggage[0] = extra_luggage; luggage[1] = 0;
        priority[0] = priority_boarding; priority[1] = 0;

        Reservation r = create_reservation(
            reservation_id,
            flight_id,
            document_number,
            seats,
            prices,
            luggage,
            priority,
            qr_code
        );

        if (!r) {
            /* se a reserva não for criada, temos de libertar porque ainda é nosso */
            free(seats); free(prices); free(luggage); free(priority);
            fprintf(ferror, "%s", line);
            continue;
        }

        reservations_manager_add(rm, r);

        /* ===== Atualizar índice da Query 6 (CORRIGIDO) ===== */
        PassengersManager pm = dataset_get_passengers(d);
        Passenger p = passengers_manager_get(pm, document_number);

        if (p) {
            const char *nat = get_passenger_nationality(p);
            if (nat && *nat) {

                /* voo final */
                const char *final_flight_id =
                    (num_ids == 2) ? flight_id[1] : flight_id[0];

                Flight f = flights_manager_get(fm, final_flight_id);

                if (f && get_flight_status(f) != Cancelled) {
                    const char *dest = get_flight_dest(f);
                    
                    if (dest && *dest) {
                        /* Aqui substituímos o acesso direto pela função pública */
                        dataset_update_q6(d, nat, dest);
                    }
                }
            }
        }

        /* ===== Atualizar estatísticas de aeroportos ===== */
        AirportsManager am = dataset_get_airports(d);
        for (int i = 0; i < num_ids; i++) {
            Flight f = flights_manager_get(fm, flight_id[i]);
            if (get_flight_status(f) != Cancelled) {
                airports_manager_departure(am, get_flight_orig(f), 1);
                airports_manager_arrival(am, get_flight_dest(f), 1);
            }
        }

        continue;

    next_line:
        continue;
    }

    fclose(f);
    fclose(ferror);
}