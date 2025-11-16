/**
 * @file airports.c
 * @brief Implementação das funções para criação e manipulação de aeroportos (Airport).
 *
 * Cada aeroporto possui código IATA, nome, cidade, país, latitude, longitude, código ICAO e tipo.
 */

#include "airports.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * @struct airport
 * @brief Estrutura que representa um aeroporto.
 */
struct airport {
    char code[4];       /**< Código IATA (3 letras). */
    char name[100];     /**< Nome do aeroporto. */
    char city[100];     /**< Cidade onde o aeroporto está localizado. */
    char country[50];   /**< País onde o aeroporto está localizado. */
    double latitude;    /**< Latitude do aeroporto. */
    double longitude;   /**< Longitude do aeroporto. */
    char icao[5];       /**< Código ICAO (4 letras). */
    char type[30];      /**< Tipo do aeroporto. */
};

/**
 * @brief Cria um novo aeroporto com os dados fornecidos.
 *
 * @param code Código IATA do aeroporto.
 * @param name Nome do aeroporto.
 * @param city Cidade do aeroporto.
 * @param country País do aeroporto.
 * @param latitude Latitude do aeroporto.
 * @param longitude Longitude do aeroporto.
 * @param icao Código ICAO do aeroporto.
 * @param type Tipo do aeroporto.
 * @return Ponteiro para o aeroporto criado ou NULL se falhar a alocação.
 */
Airport create_airport(const char *code, const char *name, const char *city, const char *country, double latitude, double longitude, const char *icao, const char *type){
    Airport a = malloc(sizeof(struct airport));
    if (!a) return NULL;

    strcpy(a->code, code);
    strcpy(a->name, name);
    strcpy(a->city, city);
    strcpy(a->country, country);
    a->latitude = latitude;
    a->longitude = longitude;
    strcpy(a->icao, icao);
    strcpy(a->type, type);

    return a;
}

/**
 * @brief Liberta a memória associada a um aeroporto.
 *
 * @param a Ponteiro para o aeroporto a destruir.
 */
void destroy_airport(Airport a){
    if (a) free(a);
}

/**
 * @brief Obtém o código IATA do aeroporto.
 *
 * @param a Ponteiro para o aeroporto.
 * @return Código IATA.
 */
const char *get_airport_code(Airport a) {
    return a->code;
}

/**
 * @brief Obtém o nome do aeroporto.
 *
 * @param a Ponteiro para o aeroporto.
 * @return Nome do aeroporto.
 */
const char *get_airport_name(Airport a) {
    return a->name;
}

/**
 * @brief Obtém a cidade onde o aeroporto está localizado.
 *
 * @param a Ponteiro para o aeroporto.
 * @return Nome da cidade.
 */
const char *get_airport_city(Airport a) {
    return a->city;
}

/**
 * @brief Obtém o país onde o aeroporto está localizado.
 *
 * @param a Ponteiro para o aeroporto.
 * @return Nome do país.
 */
const char *get_airport_country(Airport a) {
    return a->country;
}

/**
 * @brief Obtém o tipo do aeroporto.
 *
 * @param a Ponteiro para o aeroporto.
 * @return Tipo do aeroporto.
 */
const char *get_airport_type(Airport a) {
    return a->type;
}
