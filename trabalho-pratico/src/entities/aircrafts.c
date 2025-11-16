/**
 * @file aircrafts.c
 * @brief Implementação das funções para criação e manipulação de aeronaves (Aircraft).
 *
 * Cada aeronave possui identificador, fabricante, modelo, ano de fabricação,
 * capacidade máxima e alcance máximo.
 */

#include "aircrafts.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * @struct aircraft
 * @brief Estrutura que representa uma aeronave.
 */
struct aircraft {
    char id[15];          /**< Identificação da aeronave (tail number). */
    char manufacturer[50];/**< Fabricante da aeronave. */
    char model[50];       /**< Modelo da aeronave. */
    int year;             /**< Ano de fabricação. */
    int capacity;         /**< Capacidade máxima de passageiros. */
    int range;            /**< Alcance máximo em km. */
};

/**
 * @brief Cria uma nova aeronave com os dados fornecidos.
 *
 * @param id Identificador da aeronave.
 * @param manufacturer Fabricante da aeronave.
 * @param model Modelo da aeronave.
 * @param year Ano de fabricação.
 * @param capacity Capacidade máxima de passageiros.
 * @param range Alcance máximo em km.
 * @return Ponteiro para a aeronave criada ou NULL se falhar a alocação.
 */
Aircraft create_aircraft(const char *id, const char *manufacturer, const char *model, int year, int capacity, int range){
    Aircraft a = malloc(sizeof(struct aircraft));
    if(!a) return NULL;

    strcpy(a->id, id);
    strcpy(a->manufacturer, manufacturer);
    strcpy(a->model, model);
    a->year = year;
    a->capacity = capacity;
    a->range = range;

    return a;
}

/**
 * @brief Liberta a memória associada a uma aeronave.
 *
 * @param a Ponteiro para a aeronave a destruir.
 */
void destroy_aircraft (Aircraft a){
    if (a) free(a);
}

/**
 * @brief Obtém o identificador da aeronave.
 *
 * @param a Ponteiro para a aeronave.
 * @return String com o identificador da aeronave.
 */
const char *get_aircraft_id(Aircraft a) {
    return a->id;
}

/**
 * @brief Obtém o fabricante da aeronave.
 *
 * @param a Ponteiro para a aeronave.
 * @return String com o fabricante ou NULL se a aeronave for inválida.
 */
const char *get_aircraft_manufacturer(Aircraft a) {
    if (!a) return NULL;
    return a->manufacturer;
}

/**
 * @brief Obtém o modelo da aeronave.
 *
 * @param a Ponteiro para a aeronave.
 * @return String com o modelo ou NULL se a aeronave for inválida.
 */
const char *get_aircraft_model(Aircraft a) {
    if (!a) return NULL;
    return a->model;
}

/**
 * @brief Obtém o ano de fabricação da aeronave.
 *
 * @param a Ponteiro para a aeronave.
 * @return Ano de fabricação ou -1 se a aeronave for inválida.
 */
int get_aircraft_year(Aircraft a) {
    if (!a) return -1;
    return a->year;
}

/**
 * @brief Obtém a capacidade máxima da aeronave.
 *
 * @param a Ponteiro para a aeronave.
 * @return Capacidade ou -1 se a aeronave for inválida.
 */
int get_aircraft_capacity(Aircraft a) {
    if (!a) return -1;
    return a->capacity;
}

/**
 * @brief Obtém o alcance máximo da aeronave.
 *
 * @param a Ponteiro para a aeronave.
 * @return Alcance em km ou -1 se a aeronave for inválida.
 */
int get_aircraft_range(Aircraft a) {
    if (!a) return -1;
    return a->range;
}
