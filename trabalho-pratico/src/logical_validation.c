#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <glib.h>
#include "logical_validation.h"
#include "flights.h"
#include "aircrafts.h"
#include "reservations.h"
#include "passengers.h"
#include "aircrafts_manager.h"

// O campo destination de um voo, deverá ser diferente do campo origin.
bool validate_destination (Flight f){
    const char *orig = get_flight_origin(f);
    const char *dest = get_flight_destination(f);
    return strcmp(orig, dest) != 0;
}

static int compare_datetimes(const char *dt1, const char *dt2) {
    int y1, m1, d1, h1, min1;
    int y2, m2, d2, h2, min2;

    if (sscanf(dt1, "%d-%d-%d %d:%d", &y1, &m1, &d1, &h1, &min1) != 5) return 0;
    if (sscanf(dt2, "%d-%d-%d %d:%d", &y2, &m2, &d2, &h2, &min2) != 5) return 0;

    if (y1 != y2) return y1 - y2;
    if (m1 != m2) return m1 - m2;
    if (d1 != d2) return d1 - d2;
    if (h1 != h2) return h1 - h2;
    return min1 - min2;
}

//Os campos arrival e actual arrival não poderão ser anteriores aos respetivos campos departure e actual departure. Para alem disso se o status for Delayed
bool validate_arrival(Flight f) {
    const char *dep = get_flight_departure(f);
    const char *arr = get_flight_arrival(f);
    const char *act_dep = get_flight_actual_departure(f);
    const char *act_arr = get_flight_actual_arrival(f);
    flight_status status = get_flight_status(f);

    // Campos básicos têm de existir
    if (!dep || !arr || !act_dep || !act_arr)
        return false;

    // arrival ≥ departure
    if (compare_datetimes(arr, dep) < 0)
        return false;

    // actual_arrival ≥ actual_departure
    if (compare_datetimes(act_arr, act_dep) < 0)
        return false;

    // Se o voo estiver "Delayed", a actual_departure não pode ser antes da programada
    if (status == Delayed && compare_datetimes(act_dep, dep) < 0)
        return false;

    // Se o voo estiver "OnTime", as actuals devem ser >= programadas
    if (status == OnTime) {
        if (compare_datetimes(act_dep, dep) < 0) return false;
        if (compare_datetimes(act_arr, arr) < 0) return false;
    }

    // Se for Cancelled, ignora (essa regra é validada noutro sítio)
    return true;
}


// O campo aircraft de um voo, deverá corresponder a uma aeronave existente.
bool validate_aircraft(Flight f, AircraftsManager am) {
    const char *id = get_flight_aircraft_id(f);
    return aircrafts_manager_exists(am, id);
}

// Caso o campo status de um voo tome o valor Cancelled, os campos actual departure e actual arrival deverão conter o valor "N/A".
bool validate_status(Flight f){
    flight_status status = get_flight_status(f);
    const char *ad = get_flight_actual_departure(f);
    const char *aa = get_flight_actual_arrival(f);

    if (status == Cancelled)
        return strcmp(ad, "N/A") == 0 && strcmp(aa, "N/A") == 0;

    return true;
}

//O campo flight ids de uma reserva, deverá corresponder a uma lista de um ou dois voos existentes.
bool validate_reservation_flights(Reservation r, Flight *flights, int num_flights, int num_ids) {
    for (int i = 0; i < num_ids; i++) {
        bool found = false;
        const char *fid = get_reservation_flight_id(r, i);
        for (int j = 0; j < num_flights && !found; j++) {
            if (strcmp(fid, get_flight_id(flights[j])) == 0)
                found = true;
        }
        if (!found) return false;
    }
    return true;
}



//O campo document number de uma reserva, deverá corresponder a um passageiro existente.
bool logical_validate_document_number(Reservation r, Passenger *passengers, int N) {
    const char *doc = get_reservation_document_number(r);
    for (int i = 0; i < N; i++) {
        if (strcmp(doc, get_passenger_document_id(passengers[i])) == 0)
            return true;
    }
    return false;
}

// O campo flight ids de uma reserva deverá conter identificadores de voos em que o campo destination do primeiro voo seja igual ao campo origin do segundo voo
bool validate_reservation_connection(Reservation r, Flight *flights, int num_flights, int num_ids) {
    if (num_ids != 2) return true;

    Flight first = NULL, second = NULL;
    const char *fid1 = get_reservation_flight_id(r, 0);
    const char *fid2 = get_reservation_flight_id(r, 1);

    for (int i = 0; i < num_flights; i++) {
        if (strcmp(get_flight_id(flights[i]), fid1) == 0)
            first = flights[i];
        else if (strcmp(get_flight_id(flights[i]), fid2) == 0)
            second = flights[i];
    }

    if (!first || !second) return false;
    return strcmp(get_flight_destination(first), get_flight_origin(second)) == 0;
}






