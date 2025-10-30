#include "airports.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct airport {
    char code[4]; // 3 letters IATA
    char name[100]; // name of the airport
    char city[100]; // city where it's located
    char country[50]; // country where it's located
    double latitude; // latitude of the airport
    double longitude; // longitude of the aiport
    char icao[5]; // 4 letters ICAO
    char type[30]; // type of airport
};

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

void destroy_airport(Airport a){
    if (a) free(a);
}

const char *get_airport_code(Airport a) {
    return a->code;
}
