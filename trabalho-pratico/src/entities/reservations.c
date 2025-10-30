#include "reservations.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct reservation{
    char reservation_id[20]; // number of the reservation
    char flight_id[2][10]; // number of the flight
    char document_number[15]; // number of the document of identification of the passanger
    int seat[2]; // reserved seat number 
    double price [2]; // price of the reservation
    int extra_luggage[2]; // indicates with true or false (1 or 0) if the reservations includes extra luggage
    int priority_boarding[2]; // indicates with true or false (1 or 0) if the reservations include prioritary boarding
    char qr_code[50]; // associated qr code
};

Reservation create_reservation(const char *reservation_id, const char flight_id[2][10], const char *document_number, const int seat[2], const double price[2], const int extra_luggage[2], const int priority_boarding[2], const char *qr_code) {
    Reservation r = malloc(sizeof(struct reservation));
    if (!r) return NULL;

    strcpy(r->reservation_id, reservation_id);
    strcpy(r->document_number, document_number);
    strcpy(r->qr_code, qr_code);

    // Copies information for both flights
    for (int i = 0; i < 2; i++) {
        strcpy(r->flight_id[i], flight_id[i]);
        r->seat[i] = seat[i];
        r->price[i] = price[i];
        r->extra_luggage[i] = extra_luggage[i];
        r->priority_boarding[i] = priority_boarding[i];
    }

    return r;
}


void destroy_reservation(Reservation r) {
    if (r) free(r);
}

const char *get_reservation_id(Reservation r) { 
    return r->reservation_id; 
}
