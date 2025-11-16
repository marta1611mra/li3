/**
 * @file syntactic_validation.c
 * @brief Conjunto de funções para validação de datas, emails, códigos e IDs.
 *
 * Este arquivo contém funções para validar diferentes tipos de dados de entrada,
 * incluindo datas, horários, emails, códigos de aeroporto, ID de voos e reservas,
 * latitude/longitude, gênero, tipos de aeroportos e listas CSV.
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/**
 * @brief Valida uma data no formato yyyy-mm-dd.
 * 
 * Regras:
 * - Deve ter 10 caracteres.
 * - Ano, mês e dia válidos.
 * - Não pode ser posterior a 2025/09/30.
 * - Hífens nas posições corretas.
 *
 * @param date String com a data.
 * @return true se a data for válida, false caso contrário.
 */
bool validate_date(const char *date){
    int y,m,d;
    if (!date || strlen (date) != 10) return false; 
    if (sscanf(date,"%4d-%2d-%2d",&y,&m,&d) != 3) return false; 
    if (m < 1 || m > 12 || d < 1 || d > 31) return false; 
    if (y > 2025 || (y == 2025 && m > 9)) return false; 
    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) {
            if (date[i] != '-') return false; 
        } else {
            if (!isdigit((unsigned char)date[i])) return false; 
        }
    }
    return true;
}


/**
 * @brief Valida um datetime no formato yyyy-mm-dd hh:mm.
 *
 * Regras:
 * - Deve ter 16 caracteres.
 * - Ano, mês, dia, hora e minuto válidos.
 * - Não pode ser posterior a 2025/09/30.
 * - Formato com hífen, espaço e dois pontos nas posições corretas.
 *
 * @param datetime String com a data e hora.
 * @return true se o datetime for válido, false caso contrário.
 */
bool validate_datetime(const char *datetime){
    int y, m, d, h, min; 
    if (!datetime || strlen(datetime) != 16) return false; 
    if (sscanf(datetime, "%4d-%2d-%2d %2d:%2d", &y, &m, &d, &h, &min) != 5) return false; 
    if (m < 1 || m > 12 || d < 1 || d > 31) return false; 
    if (h < 0 || h > 23 || min < 0 || min > 59) return false;
    if (y > 2025 || (y == 2025 && m > 9)) return false; 
    for (int i = 0; i < 16; i++) {
        if (i == 4 || i == 7) {
            if (datetime[i] != '-') return false; 
        } else if (i == 10) {
            if (datetime[i] != ' ') return false; 
        } else if (i == 13) {
            if (datetime[i] != ':') return false; 
        } else if (!isdigit((unsigned char)datetime[i])) return false; 
    }
    return true;
}



/**
 * @brief Valida um ano no formato yyyy.
 * 
 * Regras:
 * - 4 dígitos numéricos.
 * - Não pode ser maior que 2025.
 *
 * @param year String com o ano.
 * @return true se o ano for válido, false caso contrário.
 */
bool validate_year(const char *year) {
    if (!year || strlen(year) != 4) return false;
    for (int i = 0; i < 4; i++)
        if (!isdigit((unsigned char)year[i])) return false;
    int y = atoi(year);
    if(y > 2025) return false;
    return true;
}

/**
 * @brief Valida um email no formato username@domain.
 *
 * Regras:
 * - Deve conter '@' e '.'.
 * - Username e domínio não podem estar vazios.
 * - Domínio deve terminar com 2 ou 3 letras minúsculas.
 *
 * @param email String com o email.
 * @return true se o email for válido, false caso contrário.
 */
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


/**
 * @brief Valida o código IATA de um aeroporto (3 letras maiúsculas).
 *
 * @param code Código IATA.
 * @return true se válido, false caso contrário.
 */
bool validate_airport_code(const char *code) {
    return code && strlen(code) == 3
        && code[0] >= 'A' && code[0] <= 'Z'
        && code[1] >= 'A' && code[1] <= 'Z'
        && code[2] >= 'A' && code[2] <= 'Z';
}

/**
 * @brief Valida o gênero do passageiro (M, F ou O).
 *
 * @param gender String com o gênero.
 * @return true se válido, false caso contrário.
 */
bool validate_gender(const char *gender){
    return gender && (strcmp(gender, "M") == 0 || strcmp(gender, "F") == 0 || strcmp(gender, "O") == 0);
}

/**
 * @brief Valida latitude (-90 a 90).
 *
 * @param lat String com a latitude.
 * @return true se válida, false caso contrário.
 */
bool validate_latitude(const char *lat) {
    if (!lat || !*lat) return false;
    double val; char extra;
    if (sscanf(lat, "%lf%c", &val, &extra) != 1) return false;
    if (val < -90.0 || val > 90.0) return false;
    return true;
}


/**
 * @brief Valida longitude (-180 a 180).
 *
 * @param lon String com a longitude.
 * @return true se válida, false caso contrário.
 */
bool validate_longitude(const char *lon) {
    if (!lon || !*lon) return false;
    double val; char extra;
    if (sscanf(lon, "%lf%c", &val, &extra) != 1) return false;
    if (val < -180.0 || val > 180.0) return false;
    return true;
}


/**
 * @brief Valida latitude e longitude simultaneamente.
 *
 * @param latitude String com a latitude.
 * @param longitude String com a longitude.
 * @return true se ambas forem válidas, false caso contrário.
 */
bool validate_latitude_longitude(const char *latitude,const char *longitude){
    return validate_latitude(latitude) && validate_longitude(longitude);
}

/**
 * @brief Valida o ID do voo (formato ccddddd, 2 letras + 5 dígitos).
 *
 * @param id String com o ID do voo.
 * @return true se válido, false caso contrário.
 */
bool validate_flight_id(const char *id){
    if (!id || strlen(id) != 7) return false;
    return (id[0] >= 'A' && id[0] <= 'Z') &&
           (id[1] >= 'A' && id[1] <= 'Z') &&
           isdigit(id[2]) && isdigit(id[3]) && isdigit(id[4]) &&
           isdigit(id[5]) && isdigit(id[6]);
}

/**
 * @brief Valida ID de reserva (Rnnnnnnnnn, 1 letra R + 9 dígitos).
 *
 * @param id String com o ID de reserva.
 * @return true se válido, false caso contrário.
 */
bool validate_reservation_id(const char *id){
    if (!id || strlen(id) != 10) return false;
    if ((id[0] != 'R')) return false;
    for (int i = 1; i < 10; i++) 
        if (!isdigit((unsigned char)id[i])) return false; 
    return true;
}


/**
 * @brief Valida número de documento (9 dígitos).
 *
 * @param doc_num String com o número do documento.
 * @return true se válido, false caso contrário.
 */
bool validate_document_number(const char *doc_num){
    if (!doc_num || strlen (doc_num)!= 9) return false;
    for (int i = 0; i < 9; i++) 
        if (!isdigit((unsigned char)doc_num[i])) return false; 
    return true; 
}

/**
 * @brief Valida tipo de aeroporto.
 *
 * Valores válidos:
 * - "small_airport"
 * - "medium_airport"
 * - "large_airport"
 * - "heliport"
 * - "seaplane_base"
 *
 * @param type String com o tipo de aeroporto.
 * @return true se válido, false caso contrário.
 */
bool validate_airport_type(const char *type) {
   if (!type) return false;
    return (!strcmp(type, "small_airport") ||
            !strcmp(type, "medium_airport") ||
            !strcmp(type, "large_airport") ||
            !strcmp(type, "heliport") ||
            !strcmp(type, "seaplane_base"));
}

/**
 * @brief Valida listas CSV delimitadas por colchetes [ ... ].
 *
 * @param s String com a lista CSV.
 * @return true se estiver entre colchetes, false caso contrário.
 */
bool validate_csv_lists(const char *s) {
    size_t n = strlen(s);
    return n >= 2 && s[0]=='[' && s[n-1]==']';
}

