#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "syntactic_validation.h"


// Valida uma data no formato yyyy-mm-dd.

bool validate_date(const char *date) {
    // 1. Validar tamanho e nulidade [cite: 365]
    if (!date || strlen(date) != 10) return false;

    // 2. Validar separadores '-' nas posições certas (rápido e essencial) [cite: 365]
    if (date[4] != '-' || date[7] != '-') return false;

    int y, m, d;

    // 3. O sscanf faz o parsing. Se não conseguir ler 3 inteiros, retorna false.
    if (sscanf(date, "%4d-%2d-%2d", &y, &m, &d) != 3) return false;

    // 4. Regra base: Mês 1-12 e Dia 1-31 [cite: 367]
    if (m < 1 || m > 12 || d < 1 || d > 31) return false;

    // 5. Regra da Data Atual (Limite: 2025-09-30) [cite: 269, 370]
    if (y > 2025) return false;
    if (y == 2025) {
        if (m > 9) return false;            // Meses futuros (Out, Nov, Dez)
        if (m == 9 && d > 30) return false; // Dias futuros em Setembro
    }

    return true;
}

// Valida um datetime no formato yyyy-mm-dd hh:mm.
bool validate_datetime(const char *datetime) {
    if (!datetime || strlen(datetime) != 16) return false;
    
    // Verifica separadores
    if (datetime[4] != '-' || datetime[7] != '-' || 
        datetime[10] != ' ' || datetime[13] != ':') return false;

    int y, m, d, h, min;
    if (sscanf(datetime, "%4d-%2d-%2d %2d:%2d", &y, &m, &d, &h, &min) != 5) return false;

    if (m < 1 || m > 12 || d < 1 || d > 31) return false;
    if (h < 0 || h > 23 || min < 0 || min > 59) return false;

    // Data Atual: 2025/09/30
    if (y > 2025) return false;
    if (y == 2025) {
        if (m > 9) return false;
        if (m == 9 && d > 30) return false;
    }

    return true;
}


// Valida um ano no formato yyyy.
bool validate_year(const char *year) {
    if (!year || strlen(year) != 4) return false;
    for (int i = 0; i < 4; i++)
        if (!isdigit((unsigned char)year[i])) return false;
    int y = atoi(year);
    if(y > 2025) return false;
    return true;
}


// Valida um email no formato username@domain.
bool validate_email(const char *email) {
    if (!email) return false;
    const char *at = strchr(email, '@');
    if (!at) return false;
    if (at == email) return false; 

    // username part
    for (const char *p = email; p < at; p++) {
        char c = *p;
        if (!(c == '.' || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')))
            return false;
    }

    //domain part
    const char *domain = at + 1;
    const char *dot = strchr(domain, '.');
    if (!dot) return false;

    if (dot == domain) return false; 

    int rlen = strlen(dot + 1);
    if (rlen != 2 && rlen != 3) return false;

    for (const char *p = domain; p < dot; p++)
        if (*p < 'a' || *p > 'z') return false;

    for (const char *p = dot + 1; *p; p++)
        if (*p < 'a' || *p > 'z') return false;

    return true;
}


// Valida o código IATA de um aeroporto (3 letras maiúsculas).
bool validate_airport_code(const char *code) {
    return code && strlen(code) == 3
        && code[0] >= 'A' && code[0] <= 'Z'
        && code[1] >= 'A' && code[1] <= 'Z'
        && code[2] >= 'A' && code[2] <= 'Z';
}


// Valida o gênero do passageiro (M, F ou O).
bool validate_gender(const char *gender){
    return gender && (strcmp(gender, "M") == 0 || strcmp(gender, "F") == 0 || strcmp(gender, "O") == 0);
}


// Valida latitude (-90 a 90).
bool validate_latitude(const char *lat) {
    if (!lat || !*lat) return false;
    double val; char extra;
    if (sscanf(lat, "%lf%c", &val, &extra) != 1) return false;
    if (val < -90.0 || val > 90.0) return false;
    return true;
}



// * @brief Valida longitude (-180 a 180).
bool validate_longitude(const char *lon) {
    if (!lon || !*lon) return false;
    double val; char extra;
    if (sscanf(lon, "%lf%c", &val, &extra) != 1) return false;
    if (val < -180.0 || val > 180.0) return false;
    return true;
}


// Valida latitude e longitude simultaneamente.
bool validate_latitude_longitude(const char *latitude,const char *longitude){
    return validate_latitude(latitude) && validate_longitude(longitude);
}


// Valida o ID do voo (formato ccddddd, 2 letras + 5 dígitos).
bool validate_flight_id(const char *id) {
    if (!id) return false;

    size_t len = strlen(id);
    if (len != 7 && len != 8) return false;

    /* cc */
    if (id[0] < 'A' || id[0] > 'Z') return false;
    if (id[1] < 'A' || id[1] > 'Z') return false;

    /* ddddd */
    for (size_t i = 2; i < len; i++) {
        if (!isdigit((unsigned char)id[i]))
            return false;
    }

    return true;
}


// Valida ID de reserva (Rnnnnnnnnn, 1 letra R + 9 dígitos).
bool validate_reservation_id(const char *id){
    if (!id || strlen(id) != 10) return false;
    if ((id[0] != 'R')) return false;
    for (int i = 1; i < 10; i++) 
        if (!isdigit((unsigned char)id[i])) return false; 
    return true;
}



// Valida número de documento (9 dígitos).
bool validate_document_number(const char *doc_num){
    if (!doc_num || strlen (doc_num)!= 9) return false;
    for (int i = 0; i < 9; i++) 
        if (!isdigit((unsigned char)doc_num[i])) return false; 
    return true; 
}


// Valida tipo de aeroporto.
bool validate_airport_type(const char *type) {
   if (!type) return false;
    return (!strcmp(type, "small_airport") ||
            !strcmp(type, "medium_airport") ||
            !strcmp(type, "large_airport") ||
            !strcmp(type, "heliport") ||
            !strcmp(type, "seaplane_base"));
}


// Valida listas CSV delimitadas por colchetes [ ... ].
bool validate_csv_lists(const char *s) {
    size_t n = strlen(s);
    return n >= 2 && s[0]=='[' && s[n-1]==']';
}

