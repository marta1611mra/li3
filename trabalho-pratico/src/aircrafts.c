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
}

void destroy_aircraft (Aircraft a){
    if (a) free(a);
}