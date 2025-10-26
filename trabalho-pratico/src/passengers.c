#include "passengers.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct passenger{

    char document_id[10];
    char first_name[30];
    char last_name[30];
    char dob[11];
    char nationality[20];
    char gender[10];
    char email[50];
    char phone[15];
    char address[50];
    char photo[1000];

};

Passenger create_passenger(const char *document_id, const char *first_name, const char *last_name, const char *dob, const char *nationality, const char *gender, const char *email, const char *phone, const char *address, const char *photo) {
    Passenger p = malloc(sizeof(struct passenger));
    if (!p) return NULL;

    strcpy(p->document_id, document_id);
    strcpy(p->first_name, first_name);
    strcpy(p->last_name, last_name);
    strcpy(p->dob, dob);
    strcpy(p->nationality, nationality);
    strcpy(p->gender, gender);
    strcpy(p->email, email);
    strcpy(p->phone, phone);
    strcpy(p->address, address);
    strcpy(p->photo, photo);

    return p;
}

void destroy_passenger(Passenger p) {
    if (p) free(p);
}
