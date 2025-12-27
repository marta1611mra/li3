#include "aircrafts.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


struct aircraft {
    char id[15];          // Identificação da aeronave (tail number).
    char manufacturer[50];// Fabricante da aeronave. 
    char model[50];       // Modelo da aeronave. 
    //int year;             // Ano de fabricação. 
    //int capacity;         // Capacidade máxima de passageiros. 
    //int range;            // Alcance máximo em km. 
};

// Cria uma nova aeronave com os dados fornecidos.
Aircraft create_aircraft(const char *id, const char *manufacturer, const char *model, int year, int capacity, int range){
    Aircraft a = malloc(sizeof(struct aircraft));
    if(!a) return NULL;

    strcpy(a->id, id);
    strcpy(a->manufacturer, manufacturer);
    strcpy(a->model, model);

    return a;
}

//Liberta a memória associada a uma aeronave.
void destroy_aircraft (Aircraft a){
    if (a) free(a);
}

//Obtém o identificador da aeronave.
const char *get_aircraft_id(Aircraft a) {
    return a->id;
}

//Obtém o fabricante da aeronave.
const char *get_aircraft_manufacturer(Aircraft a) {
    if (!a) return NULL;
    return a->manufacturer;
}

//Obtém o modelo da aeronave.
const char *get_aircraft_model(Aircraft a) {
    if (!a) return NULL;
    return a->model;
}


