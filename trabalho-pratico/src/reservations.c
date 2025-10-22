#include <stdio.h>

struct reservation{
    char reservation_id[20]; // number of the reservation
    char flight_id [2][10]; // number of the flight
    char document_number[15]; // number of the document of identification of the passanger
    int seat[5]; // reserved seat number 
    double price [20]; // price of the reservation
    int extra_luggage[2]; // indicates with true or false (1 or 0) if the reservations includes extra luggage
    int priority_boarding[2]; // indicates with true or false (1 or 0) if the reservations include prioritary boarding
    char qr_code[50]; // associted qr code
};