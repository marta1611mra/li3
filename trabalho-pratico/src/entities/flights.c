#include "flights.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


struct flight {
    char flight_id[9];           // Identificador do voo. 
    char departure[19];          // Partida planeada (YYYY-MM-DD HH:MM). 
    char actual_departure[19];   // Partida real (YYYY-MM-DD HH:MM ou "N/A").
    //char arrival[19];            // Chegada planeada (YYYY-MM-DD HH:MM). 
    //char actual_arrival[19];     // Chegada real (YYYY-MM-DD HH:MM ou "N/A"). 
    //char gate[6];                // Portão de embarque. 
    flight_status status;        // Status do voo (OnTime, Delayed, Cancelled). 
    char origin[4];              // Código IATA do aeroporto de origem.
    char destination[4];         // Código IATA do aeroporto de destino. 
    char aircraft_id[15];        // ID da aeronave associada. 
    char airline[40];            // Companhia aérea. 
    //char tracking_url[100];      // URL de rastreamento do voo. 
};

//Cria um novo voo com os dados fornecidos.
Flight create_flight(const char *flight_id, const char *departure, const char *actual_departure, const char *arrival, const char *actual_arrival, const char *gate, flight_status status, const char *origin, const char *destination, const char *aircraft_id, const char *airline, const char *tracking_url) {
    Flight f = malloc(sizeof(struct flight));
    if (!f) return NULL;

    strcpy(f->flight_id, flight_id);
    strcpy(f->departure, departure);
    strcpy(f->actual_departure, actual_departure);
    f->status = status;
    strcpy(f->origin, origin);
    strcpy(f->destination, destination);
    strcpy(f->aircraft_id, aircraft_id);
    strcpy(f->airline, airline);
    return f;
}

//Liberta a memória associada a um voo.
void destroy_flight(Flight f) {
    if (f) free(f);
}

//Obtém o código do aeroporto de origem do voo.
const char *get_flight_orig(Flight f) {
    return f->origin; 
}

// Obtém o código do aeroporto de destino do voo.
const char *get_flight_dest(Flight f) {
    return f->destination; 
}

// Obtém o status do voo.
flight_status get_flight_status(Flight f) {
    return f->status; 
}

// Obtém o identificador do voo.
const char *get_flight_id(Flight f) { 
    return f->flight_id; 
}

//Obtém a hora de partida planeada do voo.
const char *get_flight_dep(Flight f) { 
    return f->departure; 
}
