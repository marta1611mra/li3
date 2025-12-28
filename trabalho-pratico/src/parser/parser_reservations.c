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

// Lê e processa o ficheiro reservations.csv 
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
    
    // Buffer de I/O maior
    setvbuf(f, NULL, _IOFBF, 65536);

    exist_result();
    FILE *ferror = fopen("resultados/reservations_errors.csv", "w");
    if (!ferror) { perror("reservations_errors.csv"); fclose(f); return; }

    wcsv_header(f, ferror);

    // Obter managers uma única vez fora do loop
    PassengersManager pm = dataset_get_passengers(d);
    FlightsManager fm = dataset_get_flights(d);
    ReservationsManager rm = dataset_get_reservations(d);
    AirportsManager am = dataset_get_airports(d);

    char line[16384];  
    while (fgets(line, sizeof(line), f)) {

        char reservation_id[21] = "";
        char flight_list[128] = "";
        char document_number[16] = "";
        char seat_str[16] = "";
        char price_str[32] = "";
        char extra_luggage_str[8] = "";
        char priority_boarding_str[8] = "";
        char qr_code[512] = "";  // Reduzido de 4096 para 512 (não é usado)

        // Parsing correto
        int n = sscanf(line,
            "\"%20[^\"]\",\"%127[^\"]\",\"%15[^\"]\",\"%15[^\"]\",\"%31[^\"]\",\"%7[^\"]\",\"%7[^\"]\",\"%511[^\"]\"",
            reservation_id, flight_list, document_number, seat_str,
            price_str, extra_luggage_str, priority_boarding_str, qr_code);

        if (n != 8) {
            fprintf(ferror, "%s", line);
            continue;
        }

        remove_quotes(reservation_id); remove_spc(reservation_id);
        remove_quotes(flight_list); remove_spc(flight_list);
        remove_quotes(document_number); remove_spc(document_number);
        remove_quotes(price_str); remove_spc(price_str);
        remove_quotes(extra_luggage_str); remove_spc(extra_luggage_str);
        remove_quotes(priority_boarding_str); remove_spc(priority_boarding_str);
        // qr_code não precisa de limpeza (não é usado)

        reservation_id[strcspn(reservation_id, "\r\n")] = 0;
        flight_list[strcspn(flight_list, "\r\n")] = 0;
        document_number[strcspn(document_number, "\r\n")] = 0;
        price_str[strcspn(price_str, "\r\n")] = 0;
        extra_luggage_str[strcspn(extra_luggage_str, "\r\n")] = 0;
        priority_boarding_str[strcspn(priority_boarding_str, "\r\n")] = 0;

        // Converter price para double
        char *endptr;
        double price = strtod(price_str, &endptr);
        if (*endptr != '\0' || price < 0) {
            fprintf(ferror, "%s", line);
            continue;
        }

        if (!validate_reservation_id(reservation_id) ||
            !validate_document_number(document_number) ||
            !validate_csv_lists(flight_list)) {
            fprintf(ferror, "%s", line);
            continue;
        }

        if (!passengers_manager_exists(pm, document_number)) {
            fprintf(ferror, "%s", line);
            continue;
        }
        

        // Extrair flight IDs 
        char flight_id[2][10] = {{0}};
        int num_ids = 0;

        char list_copy[128];
        strncpy(list_copy, flight_list, sizeof(list_copy));
        list_copy[sizeof(list_copy) - 1] = '\0';

        char *start = strchr(list_copy, '[');
        char *end   = strrchr(list_copy, ']');

        if (!start || !end || start >= end) {
            fprintf(ferror, "%s", line);
            continue;
        }

        *end = '\0';
        start++;

        char *token = strtok(start, ",");

        while (token && num_ids < 2) {
            while (*token == ' ' || *token == '\'') token++;

            char *quote = strchr(token, '\'');
            if (quote) *quote = '\0';

            size_t tlen = strlen(token);
            while(tlen > 0 && isspace(token[tlen-1])) token[--tlen] = '\0';

            strncpy(flight_id[num_ids], token, 9);
            flight_id[num_ids][9] = '\0';
            num_ids++;

            token = strtok(NULL, ",");
        }

        if (num_ids == 0) {
            fprintf(ferror, "%s", line);
            continue;
        }

        // Obter voos uma única vez e reutilizar
        Flight f_first = flights_manager_get(fm, flight_id[0]);
        Flight f_last = (num_ids == 2) ? flights_manager_get(fm, flight_id[1]) : f_first;

        if (!f_first || !validate_flight_id(flight_id[0])) {
            fprintf(ferror, "%s", line);
            continue;
        }

        if (num_ids == 2) {
            if (!validate_flight_id(flight_id[1]) || !f_last) {
                fprintf(ferror, "%s", line);
                continue;
            }
            // Validação de escala
            const char *dest1 = get_flight_dest(f_first);
            const char *orig2 = get_flight_orig(f_last);
            if (!dest1 || !orig2 || strcmp(dest1, orig2) != 0) {
                fprintf(ferror, "%s", line);
                continue;
            }
        }

        int extra_luggage = (strcmp(extra_luggage_str, "true") == 0);
        int priority_boarding = (strcmp(priority_boarding_str, "true") == 0);

        int seats[2] = {0, 0};
        double prices[2] = {price, 0.0}; 
        int luggage[2] = {extra_luggage, 0};
        int priority[2] = {priority_boarding, 0};

        // Passar string vazia para qr_code (não é usado)
        Reservation r = create_reservation(
            reservation_id,
            flight_id,
            document_number,
            seats,
            prices,
            luggage,
            priority,
            ""  // qr_code vazio
        );

        if (!r) {
            fprintf(ferror, "%s", line);
            continue;
        }

        reservations_manager_add(rm, r);

        /* ===== PRÉ-PROCESSAMENTO PARA QUERIES ===== */
        
        // Obter passageiro apenas se necessário
        Passenger p = passengers_manager_get(pm, document_number);
        
        if (p) {
            const char *nat = get_passenger_nationality(p);
            
            // Q6: Atualizar índice de nacionalidade -> aeroporto de destino
            if (nat && *nat) {
                // Voo 1: Destino
                if (get_flight_status(f_first) != Cancelled) {
                    const char *dest1 = get_flight_dest(f_first);
                    if (dest1 && *dest1) dataset_update_q6(d, nat, dest1);
                }

                // Voo 2: Destino (apenas se existir e for diferente)
                if (num_ids == 2 && f_last != f_first) {
                    if (get_flight_status(f_last) != Cancelled) {
                        const char *dest2 = get_flight_dest(f_last);
                        if (dest2 && *dest2) dataset_update_q6(d, nat, dest2);
                    }
                }
            }

            // Q4: Preparar dados para cálculo de top 10 semanal
            const char *dep_str = get_flight_dep(f_first);
            if (dep_str && strlen(dep_str) >= 10) {
                dataset_add_q4_data(d, document_number, price, dep_str);
            }
        }

        // Q1: Atualizar contadores de passageiros por aeroporto
        // Verificar status e obter dados apenas uma vez
        int first_valid = (get_flight_status(f_first) != Cancelled);
        int last_valid = (get_flight_status(f_last) != Cancelled);
        
        if (first_valid) {
            const char *orig = get_flight_orig(f_first);
            if (orig && *orig) {
                // Incrementar partidas do aeroporto de origem
                airports_manager_departure(am, orig, 1);
                
                // Q3: Atualizar índice de partidas por data
                const char *actual_dep = get_flight_actual_dep(f_first);
                if (actual_dep && strlen(actual_dep) >= 10) {
                    char date_only[11];
                    memcpy(date_only, actual_dep, 10);
                    date_only[10] = '\0';
                    dataset_update_q3(d, orig, date_only);
                }
            }
            const char *dest1 = get_flight_dest(f_first);
            if (dest1 && *dest1) {
                airports_manager_arrival(am, dest1, 1);
            }
        }
        
        // Só processar se for voo diferente e válido
        if (last_valid && f_last != f_first) {
            const char *dest2 = get_flight_dest(f_last);
            if (dest2 && *dest2) {
                airports_manager_arrival(am, dest2, 1);
            }
        }
    }

    fclose(f);
    fclose(ferror);
}