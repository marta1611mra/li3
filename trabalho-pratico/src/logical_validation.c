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

// Valida se o destino de um voo é diferente da origem.
bool validate_destination(const char *orig, const char *dest) {
    if (!orig || !dest) return false;
    return strcmp(orig, dest) != 0;
}

// Função auxiliar para comparar duas datas no formato "YYYY-MM-DD HH:MM".
static int compare_datetimes(const char *dt1, const char *dt2) {
    if (!dt1 || !dt2) return 0;
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

// Valida coerência entre horários planejados e reais de um voo.
bool validate_arrival(const char *departure,
                      const char *actual_departure,
                      const char *arrival,
                      const char *actual_arrival,
                      flight_status status) {
    if (status == Cancelled) return true;
    if (!departure || !actual_departure || !arrival || !actual_arrival)
        return false;

    if (compare_datetimes(arrival, departure) < 0)
        return false;

    if (compare_datetimes(actual_arrival, actual_departure) < 0)
        return false;

    if (status == Delayed || status == OnTime) {
        if (compare_datetimes(actual_departure, departure) < 0) return false;
        if (compare_datetimes(actual_arrival, arrival) < 0) return false;
    }

    return true;
}

// Verifica se a aeronave associada a um voo existe no sistema.
bool validate_aircraft(const char *aircraft_id, AircraftsManager am) {
    if (!am || !aircraft_id || strlen(aircraft_id) == 0) return false;
    return aircrafts_manager_exists(am, aircraft_id);
}

// Valida coerência entre o estado do voo e os horários reais.
bool validate_status(flight_status status,
                     const char *actual_departure,
                     const char *actual_arrival) {
    if (!actual_departure || !actual_arrival) return false;

    if (status == Cancelled) {
        return strcmp(actual_departure, "N/A") == 0 &&
               strcmp(actual_arrival, "N/A") == 0;
    }
    return true;
}

// Valida se o documento associado à reserva corresponde a um passageiro registado.
bool logical_validate_document_number(Reservation r, Passenger *passengers, int N) {
    if (!r || !passengers) return false;
    const char *doc = get_reservation_document_number(r);
    if (!doc) return false;

    for (int i = 0; i < N; i++) {
        const char *pdoc = get_passenger_document_id(passengers[i]);
        if(pdoc && strcmp(doc,pdoc) == 0)
            return true;
    }
    return false;
}

// Valida ligação entre dois voos de uma reserva de conexão.
bool validate_reservation_connection(Reservation r, Flight *flights, int num_flights, int num_ids) {
    if (!r|| !flights) return false;
    if (num_ids != 2) return true;

    const Flight *first = NULL, *second = NULL;
    const char *fid1 = get_reservation_flight_id(r, 0);
    const char *fid2 = get_reservation_flight_id(r, 1);

    for (int i = 0; i < num_flights; i++) {
        if (strcmp(get_flight_id(flights[i]), fid1) == 0)
            first = &flights[i];
        else if (strcmp(get_flight_id(flights[i]), fid2) == 0)
            second = &flights[i];
    }

    if (!first || !second) return false;

    const char *fstdest = get_flight_dest(*first);
    const char *sndorig = get_flight_orig(*second);
    if (!fstdest || !sndorig) return false;
    return strcmp(fstdest, sndorig) == 0;
}






