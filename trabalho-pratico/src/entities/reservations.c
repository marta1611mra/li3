/**
 * @file reservations.c
 * @brief Implementação das funções para criação e manipulação de reservas (Reservation).
 *
 * Cada reserva possui identificador, até dois voos associados, número de documento do passageiro,
 * assentos, preços, bagagem extra, embarque prioritário e QR code.
 */

#include "reservations.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * @struct reservation
 * @brief Estrutura que representa uma reserva.
 */
struct reservation {
    char reservation_id[11];       /**< Número da reserva. */
    char flight_id[2][10];         /**< Identificadores dos voos associados (máx. 2). */
    char document_number[10];      /**< Número do documento de identificação do passageiro. */
    int seat[2];                   /**< Números dos assentos reservados para cada voo. */
    double price[7];               /**< Preço da reserva para cada voo (até 7?). */
    int extra_luggage[5];          /**< Indica se a reserva inclui bagagem extra (1=true, 0=false). */
    int priority_boarding[5];      /**< Indica se a reserva inclui embarque prioritário (1=true, 0=false). */
    char qr_code[4096];            /**< QR code associado à reserva. */
};

/**
 * @brief Cria uma nova reserva com os dados fornecidos.
 *
 * @param reservation_id Identificador da reserva.
 * @param flight_id Matrizes de identificadores dos voos (até 2).
 * @param document_number Número do documento do passageiro.
 * @param seat Assentos reservados para cada voo.
 * @param price Preço para cada voo.
 * @param extra_luggage Indica se há bagagem extra para cada voo.
 * @param priority_boarding Indica se há embarque prioritário para cada voo.
 * @param qr_code QR code associado à reserva.
 * @return Ponteiro para a reserva criada ou NULL se falhar a alocação.
 */
Reservation create_reservation(const char *reservation_id, const char flight_id[2][10], const char *document_number, const int seat[2], const double price[2], const int extra_luggage[2], const int priority_boarding[2], const char *qr_code) {
    Reservation r = malloc(sizeof(struct reservation));
    if (!r) return NULL;

    strcpy(r->reservation_id, reservation_id);
    strcpy(r->document_number, document_number);
    strcpy(r->qr_code, qr_code);

    for (int i = 0; i < 2; i++) {
        strcpy(r->flight_id[i], flight_id[i]);
        r->seat[i] = seat[i];
        r->price[i] = price[i];
        r->extra_luggage[i] = extra_luggage[i];
        r->priority_boarding[i] = priority_boarding[i];
    }

    return r;
}

/**
 * @brief Liberta a memória associada a uma reserva.
 *
 * @param r Ponteiro para a reserva a destruir.
 */
void destroy_reservation(Reservation r) {
    if (r) free(r);
}

/**
 * @brief Obtém o identificador da reserva.
 *
 * @param r Ponteiro para a reserva.
 * @return Identificador da reserva.
 */
const char *get_reservation_id(Reservation r) { 
    return r->reservation_id; 
}

/**
 * @brief Obtém o identificador do voo associado à reserva.
 *
 * @param r Ponteiro para a reserva.
 * @param index Índice do voo (0 ou 1).
 * @return Identificador do voo ou NULL se índice inválido.
 */
const char *get_reservation_flight_id(Reservation r, int index) {
    return (index >= 0 && index < 2) ? r->flight_id[index] : NULL;
}

/**
 * @brief Obtém o número do documento do passageiro associado à reserva.
 *
 * @param r Ponteiro para a reserva.
 * @return Número do documento do passageiro.
 */
const char *get_reservation_document_number(Reservation r) {
    return r->document_number;
}
