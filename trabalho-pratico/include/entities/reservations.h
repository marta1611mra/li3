#ifndef RESERVATIONS_H
#define RESERVATIONS_H

typedef struct reservation *Reservation;

Reservation create_reservation(const char *reservation_id, const char flight_id[2][10], const char *document_number, const int seat[2], const double price[2], const int extra_luggage[2], const int priority_boarding[2], const char *qr_code);

void destroy_reservation(Reservation r);

const char *get_reservation_id(Reservation r);

const char *get_reservation_flight_id(Reservation r, int index);

const char *get_reservation_document_number(Reservation r);
#endif