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
    printf("A ler ficheiro: %s\n", path);

    exist_result();
    FILE *ferror = fopen("resultados/reservations_errors.csv", "w");
    if (!ferror) { perror("resultados/reservations_errors.csv"); fclose(f); return; }

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

        // Leitura do CSV
        int n = sscanf(line,
            "\"%20[^\"]\",\"%127[^\"]\",\"%15[^\"]\",\"%7[^\"]\",\"%lf\",\"%7[^\"]\",\"%7[^\"]\",\"%4095[^\"]\"",
            reservation_id, flight_list, document_number, seat_str,
            &price, extra_luggage_str, priority_boarding_str, qr_code);

        if (n != 8) {
            fprintf(stderr, "sscanf falhou (n=%d) linha: %s\n", n, line);
            fprintf(ferror, "%s", line);
            continue;
        }

        // Remove aspas, espaços e quebras de linha
        remove_quotes(reservation_id); remove_spc(reservation_id);
        remove_quotes(flight_list); remove_spc(flight_list);
        remove_quotes(document_number); remove_spc(document_number);
        remove_quotes(seat_str); remove_spc(seat_str);
        remove_quotes(extra_luggage_str); remove_spc(extra_luggage_str);
        remove_quotes(priority_boarding_str); remove_spc(priority_boarding_str);
        remove_quotes(qr_code); remove_spc(qr_code);

        // Remove quebra de linha/carriage return
        reservation_id[strcspn(reservation_id, "\r\n")] = 0;
        flight_list[strcspn(flight_list, "\r\n")] = 0;
        document_number[strcspn(document_number, "\r\n")] = 0;
        seat_str[strcspn(seat_str, "\r\n")] = 0;
        extra_luggage_str[strcspn(extra_luggage_str, "\r\n")] = 0;
        priority_boarding_str[strcspn(priority_boarding_str, "\r\n")] = 0;
        qr_code[strcspn(qr_code, "\r\n")] = 0;

        // Verifica se passageiro existe
        if (!passengers_manager_exists(dataset_get_passengers(d), document_number)) {
            fprintf(ferror, "%s", line);
            continue;
        }

        // Validação da lista de voos
        if (!validate_reservation_id(reservation_id) || !validate_document_number(document_number) ||!validate_csv_lists(flight_list)) {
            fprintf(ferror, "%s", line);
            continue;
        }

        // Extrair IDs dos voos
        char flight_id[2][10] = {{0}};
        int num_ids = 0;

        char tmp[128];
        if (strlen(flight_list) > 2) {
            strncpy(tmp, flight_list + 1, strlen(flight_list) - 2); // remove colchetes
            tmp[strlen(flight_list) - 2] = '\0';

            // Substitui apóstrofos por espaços
            char cleaned[128] = "";
            int pos = 0;
            for (int i = 0; tmp[i]; i++) {
                if (tmp[i] != '\'' && tmp[i] != ' ')
                cleaned[pos++] = tmp[i];
            }
            cleaned[pos] = '\0';

            char *token = strtok(cleaned, ",");
            while (token && num_ids < 2) {
                remove_spc(token);
                strncpy(flight_id[num_ids++], token, sizeof(flight_id[0]) - 1);
                token = strtok(NULL, ",");
            }
        }
            bool valid_ids = true;
            for (int i = 0; i < num_ids; i++) {
            if (!validate_flight_id(flight_id[i])) {
            fprintf(ferror, "%s", line);
            valid_ids = false;
            break;
            }
            }
            if (!valid_ids) continue;

        if (num_ids == 0) {
            fprintf(ferror, "%s", line);
            continue;
        }

    // Verificar existência dos voos e ligação (quando há 2)
    FlightsManager fm = dataset_get_flights(d);
    bool flights_ok = true;
    for (int i = 0; i < num_ids; i++) {
        if (!flights_manager_get(fm, flight_id[i])) {
            fprintf(ferror, "%s", line);
            flights_ok = false;
            break;
        }
    }
    if (!flights_ok) continue;

    if (num_ids == 2) {
        Flight f1 = flights_manager_get(fm, flight_id[0]);
        Flight f2 = flights_manager_get(fm, flight_id[1]);
        const char *dest1 = get_flight_dest(f1);
        const char *orig2 = get_flight_orig(f2);
        if (!dest1 || !orig2 || strcmp(dest1, orig2) != 0) {
            fprintf(ferror, "%s", line);
            continue;
        }
    }

    // Converter strings lógicas
    int extra_luggage = (strcmp(extra_luggage_str, "true") == 0);
    int priority_boarding = (strcmp(priority_boarding_str, "true") == 0);


    // Criar reserva
    Reservation r = create_reservation(reservation_id, flight_id, document_number,
        (int[]){0,0}, (double[]){price,0.0},(int[]){extra_luggage,0}, (int[]){priority_boarding,0},qr_code);
        if (!r) {
            fprintf(stderr, "create_reservation falhou: %s\n", reservation_id);
            fprintf(ferror, "%s", line);
            continue;
        }
    
        AirportsManager am = dataset_get_airports(d);

        for (int i = 0; i < num_ids; i++) {
            Flight f = flights_manager_get(fm, flight_id[i]);
            if (!f) continue;

            if (get_flight_status(f) == Cancelled) continue;

            airports_manager_departure(am, get_flight_orig(f), 1);
            airports_manager_arrival(am, get_flight_dest(f), 1);
        }

        ReservationsManager rm = dataset_get_reservations(d);
        reservations_manager_add(rm, r);
    }

    fclose(f);
    fclose(ferror);
}
