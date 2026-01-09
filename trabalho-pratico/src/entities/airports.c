#include "airports.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//Estrutura que representa um aeroporto.
struct airport {
    char code[4];       // Código IATA (3 letras). 
    char name[100];     // Nome do aeroporto. 
    char city[100];     // Cidade onde o aeroporto está localizado. 
    char country[50];   // País onde o aeroporto está localizado.
    //double latitude;    // Latitude do aeroporto. 
    //double longitude;   // Longitude do aeroporto. 
    //char icao[5];       // Código ICAO (4 letras). 
    char type[30];      // Tipo do aeroporto.
    int total_passengers_arrival;
    int total_passengers_departure; 
};

// Cria um novo aeroporto com os dados fornecidos.
Airport create_airport(const char *code, const char *name, const char *city, const char *country, double latitude, double longitude, const char *icao, const char *type){
    Airport a = malloc(sizeof(struct airport));
    if (!a) return NULL;

    strcpy(a->code, code);
    strcpy(a->name, name);
    strcpy(a->city, city);
    strcpy(a->country, country);
    strcpy(a->type, type);
    a->total_passengers_arrival = 0;
    a->total_passengers_departure = 0;

    return a;
}

// Liberta a memória associada a um aeroporto.
void destroy_airport(Airport a){
    if (a) free(a);
}

// Obtém o código IATA do aeroporto.
const char *get_airport_code(Airport a) {
    return a->code;
}

//Obtém o nome do aeroporto.
const char *get_airport_name(Airport a) {
    return a->name;
}

//Obtém a cidade onde o aeroporto está localizado.
const char *get_airport_city(Airport a) {
    return a->city;
}

// Obtém o país onde o aeroporto está localizado.
const char *get_airport_country(Airport a) {
    return a->country;
}

//Obtém o tipo do aeroporto.
const char *get_airport_type(Airport a) {
    return a->type;
}

void airport_add_arrival_passengers(Airport a, int count) {
    if (a) a->total_passengers_arrival += count;
}

void airport_add_departure_passengers(Airport a, int count) {
    if (a) a->total_passengers_departure += count;
}

int get_airport_arrivals_count(Airport a) {
    return a ? a->total_passengers_arrival : 0;
}

int get_airport_departures_count(Airport a) {
    return a ? a->total_passengers_departure : 0;
}
