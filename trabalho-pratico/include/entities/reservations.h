#ifndef RESERVATIONS_H
#define RESERVATIONS_H

/**
 * @brief Opaque pointer para a estrutura Reservation.
 *
 * Representa uma reserva de voo no sistema.
 */
typedef struct reservation *Reservation;


/**
 * @brief Cria uma nova reserva.
 *
 * Inicializa um objeto Reservation com todos os campos fornecidos.
 *
 * @param reservation_id       Identificador único da reserva.
 * @param flight_id            Array de até 2 IDs de voos associados à reserva.
 * @param document_number      Número de documento do passageiro que fez a reserva.
 * @param seat                 Array com o número de lugares (até 2 voos).
 * @param price                Array com o preço por voo (até 2 voos).
 * @param extra_luggage        Array indicando quantidade de bagagem extra (até 2 voos).
 * @param priority_boarding    Array indicando prioridade de embarque (até 2 voos).
 * @param qr_code              Código QR da reserva (string).
 * @return Ponteiro para o novo Reservation criado. NULL se falhar a alocação.
 */
Reservation create_reservation(const char *reservation_id, const char flight_id[2][10], const char *document_number, const int seat[2], const double price[2], const int extra_luggage[2], const int priority_boarding[2], const char *qr_code);


/**
 * @brief Liberta a memória associada a uma reserva.
 *
 * @param r Reservation a destruir. Ignorado se for NULL.
 */
void destroy_reservation(Reservation r);

/**
 * @brief Obtém o identificador da reserva.
 *
 * @param r Reservation.
 * @return String com o ID da reserva.
 */
const char *get_reservation_id(Reservation r);

/**
 * @brief Obtém o ID de um voo associado à reserva.
 *
 * @param r     Reservation.
 * @param index Índice do voo (0 ou 1).
 * @return String com o ID do voo correspondente.
 */
const char *get_reservation_flight_id(Reservation r, int index);

/**
 * @brief Obtém o número de documento do passageiro associado à reserva.
 *
 * @param r Reservation.
 * @return String com o número de documento.
 */
const char *get_reservation_document_number(Reservation r);
#endif