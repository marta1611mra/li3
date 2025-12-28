#include "passengers.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <glib.h>

struct passenger{

    char document_id[16];
    char first_name[30];
    char last_name[30];
    char dob[11];
    char nationality[65];
    //char gender[10];
    //char email[50];
    //char phone[15];
    //char address[50];
    //char *photo;

};

//Cria um novo passageiro com os dados fornecidos.
Passenger create_passenger(const char *document_id, const char *first_name, const char *last_name, const char *dob, const char *nationality, const char *gender, const char *email, const char *phone, const char *address, const char *photo) {
    Passenger p = g_new0(struct passenger, 1);
    if (!p) return NULL;

    if (document_id)  {
        strncpy(p->document_id, document_id, sizeof(p->document_id) - 1);
        p->document_id[sizeof(p->document_id) - 1] = '\0';
    }
    if (first_name)   strncpy(p->first_name,  first_name,  sizeof(p->first_name) - 1);
    if (last_name)    strncpy(p->last_name,   last_name,   sizeof(p->last_name) - 1);
    if (dob)          strncpy(p->dob,         dob,         sizeof(p->dob) - 1);
    if (nationality)  strncpy(p->nationality, nationality, sizeof(p->nationality) - 1);

    return p;
}


// Liberta a memória associada a um passageiro.
void destroy_passenger(Passenger p) {
    if (!p) return;
        g_free(p);
}

// Obtém o número de documento do passageiro.
const char *get_passenger_document_id(Passenger p) { 
    return p->document_id; 
}

// Obtém o primeiro nome do passageiro.
const char *get_passenger_first_name(Passenger p) { 
    return p->first_name;
}
// Obtém o último nome do passageiro.
const char *get_passenger_last_name(Passenger p) { 
    return p->last_name;
}
// Obtém a data de nascimento do passageiro.
const char *get_passenger_dob(Passenger p) { 
    return p->dob;
}
// Obtém a nacionalidade do passageiro.
const char *get_passenger_nationality(Passenger p) { 
    return p->nationality;
}
