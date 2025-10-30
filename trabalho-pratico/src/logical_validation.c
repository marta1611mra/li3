#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <glib.h>
#include "flights.c"
#include "aircrafts.c"
#include "reservations.c"
#include "passengers.c"

// O campo destination de um voo, deverá ser diferente do campo origin.
bool validate_destination (struct flight f){
    return strcmp(f.destination,f.origin);
}

//Os campos arrival e actual arrival não poderão ser anteriores aos respetivos campos departure e actual departure. Para alem disso se o status for Delayed
bool validate_arrival(struct flight f){
    int d1,m1,y1,h1,min1;
    int d2,m2,y2,h2,min2;
    int d3,m3,y3,h3,min3;
    int d4,m4,y4,h4,min4;


    sscanf(f.departure, "%d-%d-%d %d:%d", &y1, &m1, &d1, &h1, &min1);
    sscanf(f.actual_departure, "%d-%d-%d %d:%d", &y2, &m2, &d2, &h2, &min2);
    sscanf(f.arrival, "%d-%d-%d %d:%d", &y3, &m3, &d3, &h3, &min3);
    sscanf(f.actual_arrival, "%d-%d-%d %d:%d", &y4, &m4, &d4, &h4, &min4);

    if (f.status == Delayed) {

        if (y2 < y1) return false;
        if (y2 == y1 && m2 < m1) return false;
        if (y2 == y1 && m2 == m1 && d2 < d1) return false;
        if (y2 == y1 && m2 == m1 && d2 == d1 && h2 < h1) return false;
        if (y2 == y1 && m2 == m1 && d2 == d1 && h2 == h1 && min2 < min1) return false;
    }

    if (f.status==OnTime){
    if (y1>y3) return false;
    if (y2>y4) return false;
    if (y1 == y3 && m1 > m3) return false;
    if (y1 == y3 && m1 == m3 && d1 > d3) return false;
    if (y1 == y3 && m1 == m3 && d3 == d1 && h1 > h3) return false;
    if (y1 == y3 && m1 == m3 && d3 == d1 && h1 == h3 && min1 > min3) return false;
    if (y2==y4 && m2 > m4) return false;
    if (y2==y4 && m2==m4 && d2 > d4) return false;
    if (y2==y4 && m2 ==m4 && d2==d4 && h2> h4) return false;
    if (y2==y4 && m2==m4 && d2==d4 && h2==h4 && min2 > min4) return false;
    }

    return true;
    
}

// O campo aircraft de um voo, deverá corresponder a uma aeronave existente.
bool validate_aircraft(struct flight f, GHashTable *aircrafts) {
    if (!aircrafts) return false;
    if (strlen(f.aircraft_id) == 0) return false;

    // Verifica se o aircraft_id do voo existe na hash table
    gpointer found = g_hash_table_lookup(aircrafts, f.aircraft_id);
    return (found != NULL);
}

// Caso o campo status de um voo tome o valor Cancelled, os campos actual departure e actual arrival deverão conter o valor "N/A".
bool validate_status(struct flight f){
    if (f.status== Cancelled) return (strcmp(f.actual_departure,"N/A")==0 && strcmp(f.actual_arrival,"N/A")==0);
    return false;
}

//O campo flight ids de uma reserva, deverá corresponder a uma lista de um ou dois voos existentes.
bool validate_reservation_flights(struct reservation r, struct flight flights[], int num_flights, int num_flights_in_reservation) {
    for (int i = 0; i < num_flights_in_reservation; i++) {
        bool found = false;
        for (int j = 0; j < num_flights && !found; j++) {
            if (strcmp(r.flight_id[i], flights[j].flight_id) == 0) {
                found = true;
            }
        }
        if (!found)
            return false; 
    }
    return true; 
}


//O campo document number de uma reserva, deverá corresponder a um passageiro existente.
bool logical_validate_document_number(struct reservation r, struct passenger p[], int N) {
    for (int i = 0; i < N; i++) {
        if (strcmp(r.document_number, p[i].document_id) == 0) {
            return true; 
        }
    }
    return false; 
}

// O campo flight ids de uma reserva deverá conter identificadores de voos em que o campo destination do primeiro voo seja igual ao campo origin do segundo voo
bool validate_reservation_connection(struct reservation r, struct flight flights[], int num_flights, int num_flights_in_reservation) {

    if (num_flights_in_reservation != 2) 
        return true;

    struct flight *first = NULL;
    struct flight *second = NULL;

    for (int i = 0; i < num_flights; i++) {
        if (strcmp(flights[i].flight_id, r.flight_id[0]) == 0)
            first = &flights[i];
        else if (strcmp(flights[i].flight_id, r.flight_id[1]) == 0)
            second = &flights[i];
    }


    if (!first || !second)
        return false;

    return strcmp(first->destination, second->origin) == 0;
}





