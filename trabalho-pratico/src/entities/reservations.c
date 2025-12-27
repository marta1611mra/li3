#include "reservations.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct reservation {
    char reservation_id[11];       /**Número da reserva. */
    char flight_id[2][10];         /**Identificadores dos voos associados (máx. 2). */
    char document_number[10];      /**Número do documento de identificação do passageiro. */
    //int seat[2];                   /**Números dos assentos reservados para cada voo. */
    double price[2];               /**Preço da reserva para cada voo. */
    //int extra_luggage[2];          /**Indica se a reserva inclui bagagem extra (1=true, 0=false). */
    //int priority_boarding[2];      /**Indica se a reserva inclui embarque prioritário (1=true, 0=false). */
    //char *qr_code;            /**QR code associado à reserva. */
};


// Cria uma nova reserva com os dados fornecidos.
Reservation create_reservation(const char *reservation_id, const char flight_id[2][10], const char *document_number, const int seat[2], const double price[2], const int extra_luggage[2], const int priority_boarding[2], const char *qr_code) {
    Reservation r = malloc(sizeof(struct reservation));
    if (!r) return NULL;

    strcpy(r->reservation_id, reservation_id);
    strcpy(r->document_number, document_number);

    for (int i = 0; i < 2; i++) {
        strcpy(r->flight_id[i], flight_id[i]);
        r->price[i] = price[i];
    }

    return r;
}

// Liberta a memória associada a uma reserva.
void destroy_reservation(Reservation r) {
    if (!r) return;
    free(r);
}

// Obtém o identificador da reserva.
const char *get_reservation_id(Reservation r) { 
    return r->reservation_id; 
}

// Obtém o identificador do voo associado à reserva.
const char *get_reservation_flight_id(Reservation r, int index) {
    return (index >= 0 && index < 2) ? r->flight_id[index] : NULL;
}

// Obtém o número do documento do passageiro associado à reserva.
const char *get_reservation_document_number(Reservation r) {
    return r->document_number;
}
// Obtém o preço pago na reserva para um voo específico.
double get_reservation_price(Reservation r, int index) {
    return (index >= 0 && index < 2) ? r->price[index] : 0.0;
}

// Obtém o preço total pago na reserva.
double get_reservation_total_price(Reservation r) {
    double total = 0.0;
    for (int i = 0; i < 2; i++) {
        total += r->price[i];
    }
    return total;
}

