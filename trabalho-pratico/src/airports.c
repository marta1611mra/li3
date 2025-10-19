#include <stdio.h>

struct airport {
    char code[4]; // 3 letters IATA
    char name[100]; // name of the airport
    char city[100]; // city where it's located
    char country[50]; // country where it's located
    double latitude[50]; // latitude of the airport
    double longitude[50]; // longitude of the aiport
    char icao[5]; // 4 letters ICAO
    char type[30]; // type of airport
};

