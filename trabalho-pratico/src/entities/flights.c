/**
 * @file flights.c
 * @brief Implementação das funções para criação e manipulação de voos (Flight).
 *
 * Cada voo possui identificador, horários de partida e chegada (planejados e reais),
 * portão, status, origem, destino, aeronave associada, companhia aérea e URL de rastreamento.
 */

#include "flights.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * @struct flight
 * @brief Estrutura que representa um voo.
 */
struct flight {
    char flight_id[8];           /**< Identificador do voo. */
    char departure[19];          /**< Partida planeada (YYYY-MM-DD HH:MM). */
    char actual_departure[19];   /**< Partida real (YYYY-MM-DD HH:MM ou "N/A"). */
    char arrival[19];            /**< Chegada planeada (YYYY-MM-DD HH:MM). */
    char actual_arrival[19];     /**< Chegada real (YYYY-MM-DD HH:MM ou "N/A"). */
    char gate[6];                /**< Portão de embarque. */
    flight_status status;        /**< Status do voo (OnTime, Delayed, Cancelled). */
    char origin[4];              /**< Código IATA do aeroporto de origem. */
    char destination[4];         /**< Código IATA do aeroporto de destino. */
    char aircraft_id[15];        /**< ID da aeronave associada. */
    char airline[40];            /**< Companhia aérea. */
    char tracking_url[100];      /**< URL de rastreamento do voo. */
};

/**
 * @brief Cria um novo voo com os dados fornecidos.
 *
 * @param flight_id Identificador do voo.
 * @param departure Partida planeada (YYYY-MM-DD HH:MM).
 * @param actual_departure Partida real (YYYY-MM-DD HH:MM ou "N/A").
 * @param arrival Chegada planeada (YYYY-MM-DD HH:MM).
 * @param actual_arrival Chegada real (YYYY-MM-DD HH:MM ou "N/A").
 * @param gate Portão do voo.
 * @param status Status do voo (OnTime, Delayed, Cancelled).
 * @param origin Código IATA do aeroporto de origem.
 * @param destination Código IATA do aeroporto de destino.
 * @param aircraft_id ID da aeronave associada.
 * @param airline Companhia aérea.
 * @param tracking_url URL de rastreamento do voo.
 * @return Ponteiro para o voo criado ou NULL se falhar a alocação.
 */
Flight create_flight(const char *flight_id, const char *departure, const char *actual_departure, const char *arrival, const char *actual_arrival, const char *gate, flight_status status, const char *origin, const char *destination, const char *aircraft_id, const char *airline, const char *tracking_url) {
    Flight f = malloc(sizeof(struct flight));
    if (!f) return NULL;

    strcpy(f->flight_id, flight_id);
    strcpy(f->departure, departure);
    strcpy(f->actual_departure, actual_departure);
    strcpy(f->arrival, arrival);
    strcpy(f->actual_arrival, actual_arrival);
    strncpy(f->gate, gate, sizeof(f->gate)-1);
    f->gate[sizeof(f->gate)-1] = '\0';  
    f->status = status;
    strcpy(f->origin, origin);
    strcpy(f->destination, destination);
    strcpy(f->aircraft_id, aircraft_id);
    strcpy(f->airline, airline);
    strcpy(f->tracking_url, tracking_url);

    return f;
}

/**
 * @brief Liberta a memória associada a um voo.
 *
 * @param f Ponteiro para o voo a destruir.
 */
void destroy_flight(Flight f) {
    if (f) free(f);
}

/**
 * @brief Obtém o código do aeroporto de origem do voo.
 *
 * @param f Ponteiro para o voo.
 * @return Código IATA do aeroporto de origem.
 */
const char *get_flight_orig(Flight f) {
    return f->origin; 
}

/**
 * @brief Obtém o código do aeroporto de destino do voo.
 *
 * @param f Ponteiro para o voo.
 * @return Código IATA do aeroporto de destino.
 */
const char *get_flight_dest(Flight f) {
    return f->destination; 
}

/**
 * @brief Obtém o status do voo.
 *
 * @param f Ponteiro para o voo.
 * @return Status do voo (OnTime, Delayed, Cancelled).
 */
flight_status get_flight_status(Flight f) {
    return f->status; 
}

/**
 * @brief Obtém o identificador do voo.
 *
 * @param f Ponteiro para o voo.
 * @return String com o identificador do voo.
 */
const char *get_flight_id(Flight f) { 
    return f->flight_id; 
}

/**
 * @brief Obtém a hora de partida planeada do voo.
 *
 * @param f Ponteiro para o voo.
 * @return String com a partida planeada (YYYY-MM-DD HH:MM).
 */
const char *get_flight_dep(Flight f) { 
    return f->departure; 
}

/**
 * @brief Obtém a hora de partida real do voo.
 *
 * @param f Ponteiro para o voo.
 * @return String com a partida real (YYYY-MM-DD HH:MM ou "N/A").
 */
const char *get_flight_actual_dep(Flight f) { 
    return f->actual_departure; 
}

/**
 * @brief Obtém a hora de chegada planeada do voo.
 *
 * @param f Ponteiro para o voo.
 * @return String com a chegada planeada (YYYY-MM-DD HH:MM).
 */
const char *get_flight_arrival(Flight f) { 
    return f->arrival; 
}

/**
 * @brief Obtém a hora de chegada real do voo.
 *
 * @param f Ponteiro para o voo.
 * @return String com a chegada real (YYYY-MM-DD HH:MM ou "N/A").
 */
const char *get_flight_actual_ar(Flight f) { 
    return f->actual_arrival; 
}

/**
 * @brief Obtém o portão do voo.
 *
 * @param f Ponteiro para o voo.
 * @return String com o portão do voo.
 */
const char *get_flight_gate(Flight f) { 
    return f->gate; 
}

/**
 * @brief Obtém o ID da aeronave associada ao voo.
 *
 * @param f Ponteiro para o voo.
 * @return ID da aeronave.
 */
const char *get_flight_aircraft_id(Flight f) { 
    return f->aircraft_id; 
}

/**
 * @brief Obtém a companhia aérea do voo.
 *
 * @param f Ponteiro para o voo.
 * @return String com a companhia aérea.
 */
const char *get_flight_airline(Flight f) { 
    return f->airline; 
}
