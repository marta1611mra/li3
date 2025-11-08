#include "aircrafts.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct aircraft {
    char id[15]; // identification (tail number)
    char manufacturer[50]; // manufacturer of the aircraft
    char model[50]; // model of the aircraft
    int year; // year of fabrication
    int capacity; // max capacity of the aircraft
    int range; // maximum range in km's
};

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

void destroy_aircraft (Aircraft a){
    if (a) free(a);
}

const char *get_aircraft_id(Aircraft a) {
    return a->id;
}

const char *get_aircraft_manufacturer(Aircraft a) {
    if (!a) return NULL;
    return a->manufacturer;
}

const char *get_aircraft_model(Aircraft a) {
    if (!a) return NULL;
    return a->model;
}

int get_aircraft_year(Aircraft a) {
    if (!a) return -1;
    return a->year;
}

int get_aircraft_capacity(Aircraft a) {
    if (!a) return -1;
    return a->capacity;
}

int get_aircraft_range(Aircraft a) {
    if (!a) return -1;
    return a->range;
}