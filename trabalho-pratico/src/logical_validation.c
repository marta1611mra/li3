#include <stdio.h>
#include <stdbool.h>
#include <string.h>
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
bool validate_aircraft(struct flight f, struct aircraft a[], int N) {
    for (int i = 0; i < N; i++) {
        if (strcmp(f.aircraft_id, (a[i]).id) == 0) {
            return true;
        }
    }
    return false; 
}

// Caso o campo status de um voo tome o valor Cancelled, os campos actual departure e actual arrival deverão conter o valor "N/A".
bool validade_status(struct flight f){
    if (f.status== Cancelled) return (strcmp(f.actual_departure,"N/A")==0 && strcmp(f.actual_arrival,"N/A")==0);
    return false;
}

//O campo flight ids de uma reserva, deverá corresponder a uma lista de um ou dois voos existentes.





//O campo document number de uma reserva, deverá corresponder a um passageiro existente.
bool validate_document_number(struct reservation r, struct passenger p[], int N) {
    for (int i = 0; i < N; i++) {
        if (strcmp(r.document_number, p[i].document_id) == 0) {
            return true; 
        }
    }
    return false; 
}

// O campo flight ids de uma reserva deverá conter identificadores de voos em que o campo destination do primeiro voo seja igual ao campo origin do segundo voo





