#include "parser.h"
#include "dataset.h"
#include "syntactic_validation.h"
#include "logical_validation.h"
#include "flights.h"
#include "reservations.h"
#include "passengers.h"
#include "aircrafts.h"
#include "airports.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <glib.h>

// Helper para verificar se uma string contém apenas dígitos
static bool is_all_digits(const char *s) {
    if (!s || !*s) return false;
    for (const char *p = s; *p; p++) {
        if (!isdigit((unsigned char)*p)) return false;
    }
    return true;
}

// Removes quotation marks 
static void remove_quotes(char *str) {
    if(!str) return;
    size_t len = strlen(str);
    if (len == 0) return;
    if (str[0] == '"') memmove(str, str + 1, len);
    len = strlen(str);
    if (len > 0 && str[len - 1] == '"') str[len - 1] = '\0';
}

static void remove_spc(char *str) {
    if (!str) return;
    int len = (int)strlen(str);
    
    while (isspace((unsigned char)*str)) memmove(str, str + 1, strlen(str));
    
    while (len > 0 && (isspace((unsigned char)str[len - 1]) ||
                       str[len - 1] == '\r' || str[len - 1] == '\n')) {
        str[--len] = '\0';
    }
}

// creates the directory resultados if it doesn't exists
static void exist_result(void) {
    system ("mkdir -p resultados");
}

// helps writting header line to error file
static void wcsv_header(FILE *csv, FILE *ferror) {
    char header[2048];
    if(fgets(header, sizeof(header), csv)) {
        fprintf(ferror, "%s", header);
    }
}

void parse_airports(Dataset d, const char *data_path) {
    char path[512];
    snprintf(path, sizeof(path), "%s/airports.csv", data_path);

    FILE *f = fopen(path, "r");
    if (!f) { perror(path); return; }
    printf("📄 A ler ficheiro: %s\n", path);

    exist_result();
    FILE *ferror = fopen("resultados/airports_errors.csv", "w");
    if (!ferror) { perror("resultados/airports_errors.csv"); fclose(f); return; }

    wcsv_header(f, ferror);

    char line[2048];
    while (fgets(line, sizeof(line), f)) {
        char code[4] = "", name[101] = "", city[101] = "", country[51] = "";
        char lat_str[17] = "", long_str[17] = "", icao[5] = "", type[31] = "";

        int n = sscanf(line,
            "\"%3[^\"]\",\"%100[^\"]\",\"%100[^\"]\",\"%50[^\"]\",\"%16[^\"]\",\"%16[^\"]\",\"%4[^\"]\",\"%30[^\"]\"",
            code, name, city, country, lat_str, long_str, icao, type);

        if (n < 8) { 
            fprintf(ferror, "%s", line); 
            continue; 
        }

        remove_quotes(code); remove_spc(code);
        remove_quotes(name); remove_spc(name);
        remove_quotes(city); remove_spc(city);
        remove_quotes(country); remove_spc(country);
        remove_quotes(lat_str); remove_spc(lat_str);
        remove_quotes(long_str); remove_spc(long_str);
        remove_quotes(icao); remove_spc(icao);
        remove_quotes(type); remove_spc(type);

        if (!validate_airport_code(code) ||
            !validate_latitude_longitude(lat_str, long_str) || !validate_airport_type(type) ||
            strlen(name) == 0) {
            fprintf(ferror, "%s", line);
            continue;
        }

        double latitude = strtod(lat_str, NULL);
        double longitude = strtod(long_str, NULL);

        Airport a = create_airport(code, name, city, country, latitude, longitude, icao, type);
        if (!a) {fprintf(stderr, "create_airport failed\n"); fprintf(ferror, "%s", line); continue; } 
    
        AirportsManager am = dataset_get_airports(d);
        airports_manager_add(am, a);
    }

    fclose(f);
    fclose(ferror);
}


void parse_aircrafts(Dataset d, const char *data_path) {
    char path[512];
    snprintf(path, sizeof(path), "%s/aircrafts.csv", data_path);

    FILE *f = fopen(path, "r");
    if (!f) { perror(path); return; }
    printf("📄 A ler ficheiro: %s\n", path);

    exist_result();
    FILE *ferror = fopen("resultados/aircrafts_errors.csv", "w");
    if (!ferror) { perror("resultados/aircrafts_errors.csv"); fclose(f); return; }

    wcsv_header(f, ferror);

    char line[2048];
    while (fgets(line, sizeof(line), f)) {
        char id[16] = "", manufacturer[51] = "", model[51] = "";
        char year_str[8] = "", capacity_str[8] = "", range_str[10] = "";

        int n = sscanf(line,
                "\"%15[^\"]\",\"%50[^\"]\",\"%50[^\"]\",\"%7[^\"]\",\"%7[^\"]\",\"%9[^\"]\"",
                id, manufacturer, model, year_str, capacity_str, range_str);

        if (n < 6) { fprintf(ferror, "%s", line); continue; }

        remove_quotes(id); remove_spc(id);
        remove_quotes(manufacturer); remove_spc(manufacturer);
        remove_quotes(model); remove_spc(model);
        remove_quotes(year_str); remove_spc(year_str);
        remove_quotes(capacity_str); remove_spc(capacity_str);
        remove_quotes(range_str); remove_spc(range_str);

// Validação sintática dos campos
if (strlen(id) == 0 ||
    !validate_year(year_str) ||
    !is_all_digits(capacity_str) || atoi(capacity_str) <= 0 ||
    !is_all_digits(range_str) || atoi(range_str) <= 0) {
    fprintf(ferror, "%s", line);
    continue;
}

        // Conversão segura após validação
        int year = atoi(year_str);
        int capacity = atoi(capacity_str);
        int range = atoi(range_str);

        Aircraft a = create_aircraft(id, manufacturer, model, year, capacity, range);
        if (!a) {fprintf(stderr, "create_aircraft failed\n"); fprintf(ferror, "%s", line); continue; }

        AircraftsManager am = dataset_get_aircrafts(d);
        aircrafts_manager_add(am, a);
    }

    fclose(f);
    fclose(ferror);
}

void parse_flights(Dataset d, const char *data_path) {
    char path[512];
    snprintf(path, sizeof(path), "%s/flights.csv", data_path);

    FILE *f = fopen(path, "r");
    if (!f) { perror(path); return; }
    printf("📄 A ler ficheiro: %s\n", path);

    exist_result();
    FILE *ferror = fopen("resultados/flights_errors.csv", "w");
    if (!ferror) { perror("resultados/flights_errors.csv"); fclose(f); return; }

    wcsv_header(f, ferror);

    char line[2048];
    while (fgets(line, sizeof(line), f)) {
        char flight_id[9] = "", departure[20] = "", actual_departure[20] = "", arrival[20] = "", actual_arrival[20] = "";
        char gate[4] = "", origin[5] = "", destination[5] = "", aircraft_id[16] = "", airline[41] = "", tracking_url[101] = "", status_str[16] = "";

        int n = sscanf(line,
                "\"%8[^\"]\",\"%19[^\"]\",\"%19[^\"]\",\"%19[^\"]\",\"%19[^\"]\",\"%3[^\"]\",\"%15[^\"]\",\"%4[^\"]\",\"%4[^\"]\",\"%15[^\"]\",\"%40[^\"]\",\"%100[^\"]\"",
                flight_id, departure, actual_departure, arrival, actual_arrival,
                gate, status_str, origin, destination, aircraft_id, airline, tracking_url);


        if (n < 12) { fprintf(ferror, "%s", line); continue; }

        remove_quotes(flight_id); remove_spc(flight_id);
        remove_quotes(aircraft_id); remove_spc(aircraft_id);
        remove_quotes(status_str); remove_spc(status_str);
        remove_quotes(origin); remove_spc(origin);
        remove_quotes(destination); remove_spc(destination);
        flight_status status;

 // Aceita "On Time" e "OnTime"
if (strcmp(status_str, "On Time") == 0 || strcmp(status_str, "OnTime") == 0)
    status = OnTime;
else if (strcmp(status_str, "Delayed") == 0)
    status = Delayed;
else if (strcmp(status_str, "Cancelled") == 0)
    status = Cancelled;
else {
    fprintf(ferror, "%s", line);
    continue;
}

// --- Validação sintática e lógica ---
bool synt_ok =
    validate_flight_id(flight_id) &&
    validate_datetime(departure) &&
    validate_datetime(arrival) &&
    validate_airport_code(origin) &&
    validate_airport_code(destination) &&
    validate_aircraft(aircraft_id, dataset_get_aircrafts(d)) &&
    validate_destination(origin, destination);

if (status == Cancelled) {
    // "Cancelled" → actual_* devem ser "N/A"
    synt_ok = synt_ok &&
              strcmp(actual_departure, "N/A") == 0 &&
              strcmp(actual_arrival, "N/A") == 0;
} else {
    synt_ok = synt_ok &&
              validate_datetime(actual_departure) &&
              validate_datetime(actual_arrival);
}

if (!synt_ok ||
    !validate_arrival(departure, actual_departure, arrival, actual_arrival, status) ||
    !validate_status(status, actual_departure, actual_arrival)) {
    fprintf(ferror, "%s", line);
    continue;
}
        
        Flight fv = create_flight(flight_id, departure, actual_departure, arrival,
                                  actual_arrival, gate, status, origin,
                                  destination, aircraft_id, airline, tracking_url);
        if (!fv) {
        fprintf(stderr, "create_flight failed\n");
        fprintf(ferror, "%s", line);
        continue;
        }
        FlightsManager fm = dataset_get_flights(d);
        flights_manager_add(fm, fv);
    }

    fclose(f);
    fclose(ferror);
}

void parse_passengers(Dataset d, const char *data_path) {
    char path[512];
    snprintf(path, sizeof(path), "%s/passengers.csv", data_path);

    FILE *f = fopen(path, "r");
    if (!f) { perror(path); return; }
    printf("📄 A ler ficheiro: %s\n", path);

    exist_result();
    FILE *ferror = fopen("resultados/passengers_errors.csv", "w");
    if (!ferror) { perror("resultados/passengers_errors.csv"); fclose(f); return; }

    wcsv_header(f, ferror);

    char line[2048];
    while (fgets(line, sizeof(line), f)) {
        char document_id[11] = "", first_name[31] = "", last_name[31] = "", dob[12] = "";
        char nationality[21] = "", gender[11] = "", email[51] = "", phone[16] = "", address[51] = "", photo[1001] = "";

        int n = sscanf(line,
                "\"%10[^\"]\",\"%30[^\"]\",\"%30[^\"]\",\"%11[^\"]\",\"%20[^\"]\",\"%10[^\"]\",\"%50[^\"]\",\"%15[^\"]\",\"%50[^\"]\",\"%1000[^\"]\"",
                document_id, first_name, last_name, dob, nationality,
                gender, email, phone, address, photo);


        if (n < 10) { fprintf(ferror, "%s", line); continue; }

        remove_quotes(document_id); remove_spc(document_id);
        remove_quotes(first_name); remove_spc(first_name);
        remove_quotes(last_name); remove_spc(last_name);
        remove_quotes(dob); remove_spc(dob);
        remove_quotes(nationality); remove_spc(nationality);
        remove_quotes(gender); remove_spc(gender);
        remove_quotes(email); remove_spc(email);
        remove_quotes(phone); remove_spc(phone);
        remove_quotes(address); remove_spc(address);
        remove_quotes(photo); remove_spc(photo);

        if (!validate_email(email) ||
            !validate_document_number(document_id) ||
            !validate_gender(gender) ||
            !validate_date(dob)) {
            fprintf(ferror, "%s", line);
            continue;
        }

        Passenger p = create_passenger(document_id, first_name, last_name, dob,
                                       nationality, gender, email, phone, address, photo);
        if (!p) { fprintf(stderr, "create_passenger failed"); fprintf(ferror, "%s", line); continue; }

        PassengersManager pm = dataset_get_passengers(d);
        passengers_manager_add(pm, p);
        }

    fclose(f);
    fclose(ferror);
}


void parse_reservations(Dataset d, const char *data_path) {
    char path[512];
    snprintf(path, sizeof(path), "%s/reservations.csv", data_path);

    FILE *f = fopen(path, "r");
    if (!f) { perror(path); return; }
    printf("📄 A ler ficheiro: %s\n", path);

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
        char qr_code[1001] = "";
        double price = 0.0;

        // Leitura do CSV
        int n = sscanf(line,
            "\"%20[^\"]\",\"%127[^\"]\",\"%15[^\"]\",\"%7[^\"]\",\"%lf\",\"%7[^\"]\",\"%7[^\"]\",\"%1000[^\"]\"",
            reservation_id, flight_list, document_number, seat_str,
            &price, extra_luggage_str, priority_boarding_str, qr_code);

        if (n != 8) {
            fprintf(stderr, "❌ DEBUG sscanf falhou (n=%d) linha: %s\n", n, line);
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

        // Debug: conferir document_number
        // printf("DEBUG document_number='%s'\n", document_number);

        // Verifica se passageiro existe
        if (!passengers_manager_exists(dataset_get_passengers(d), document_number)) {
            fprintf(stderr, "❌ DEBUG passageiro inexistente: %s\n", document_number);
            fprintf(ferror, "%s", line);
            continue;
        }

        // Validação da lista de voos
        if (!validate_reservation_id(reservation_id) || !validate_document_number(document_number) ||!validate_csv_lists(flight_list)) {
            fprintf(stderr, "❌ DEBUG lista de voos inválida: %s\n", flight_list);
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

        if (num_ids == 0) {
            fprintf(stderr, "❌ DEBUG não encontrou flight_id em %s\n", flight_list);
            fprintf(ferror, "%s", line);
            continue;
        }

        // Verificar existência dos voos e ligação (quando há 2)
FlightsManager fm = dataset_get_flights(d);
bool flights_ok = true;
for (int i = 0; i < num_ids; i++) {
    if (!flights_manager_get(fm, flight_id[i])) {
        fprintf(stderr, "❌ DEBUG voo inexistente: %s\n", flight_id[i]);
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
        fprintf(stderr, "❌ DEBUG ligação inválida entre %s e %s\n", flight_id[0], flight_id[1]);
        fprintf(ferror, "%s", line);
        continue;
    }
}

        // Converter strings lógicas
        int extra_luggage = (strcmp(extra_luggage_str, "true") == 0);
        int priority_boarding = (strcmp(priority_boarding_str, "true") == 0);

        // Criar reserva
        Reservation r = create_reservation(reservation_id, flight_id, document_number,
                                           (int[]){0,0}, (double[]){price,0.0},
                                           (int[]){extra_luggage,0}, (int[]){priority_boarding,0},
                                           qr_code);
        if (!r) {
            fprintf(stderr, "❌ DEBUG create_reservation falhou: %s\n", reservation_id);
            fprintf(ferror, "%s", line);
            continue;
        }

        fprintf(stderr, "✅ DEBUG reserva válida: %s -> %s | %s\n",
                reservation_id, flight_id[0], document_number);

        ReservationsManager rm = dataset_get_reservations(d);
        reservations_manager_add(rm, r);
    }

    fclose(f);
    fclose(ferror);
}
