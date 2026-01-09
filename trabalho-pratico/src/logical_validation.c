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
    return strcmp(dt1, dt2);
}

// Valida coerência entre horários planejados e reais de um voo.
bool validate_arrival(const char *departure,
                      const char *actual_departure,
                      const char *arrival,
                      const char *actual_arrival,
                      flight_status status) {
    if (compare_datetimes(arrival, departure) < 0)
        return false;
    if (status == Cancelled) return true;
    if (!departure || !actual_departure || !arrival || !actual_arrival)
        return false;
    if (compare_datetimes(actual_departure, departure) < 0) return false;
    if (compare_datetimes(actual_arrival, arrival) < 0) return false;

    // Lógica extra: Chegada Real tem de ser depois da Partida Real
    if (compare_datetimes(actual_arrival, actual_departure) < 0) return false;
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
