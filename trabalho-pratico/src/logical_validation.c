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

// the destination of a flight must be different from its origin
bool validate_destination (Flight f){
    if (!f) return false;
    const char *orig = get_flight_orig(f);
    const char *dest = get_flight_dest(f);
    if (!orig || !dest) return false;
    return strcmp(orig, dest) != 0;
}

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

// arrival and actual_arrival can't be before departure and actual_departure
bool validate_arrival(Flight f) {
    if (!f) return false;

    const char *dep = get_flight_dep(f);
    const char *arr = get_flight_arrival(f);
    const char *act_dep = get_flight_actual_dep(f);
    const char *act_arr = get_flight_actual_ar(f);
    flight_status status = get_flight_status(f);

    if (!dep || !arr || !act_dep || !act_arr)
        return false;

    // arrival ≥ departure
    if (compare_datetimes(arr, dep) < 0)
        return false;

    // actual_arrival ≥ actual_departure
    if (compare_datetimes(act_arr, act_dep) < 0)
        return false;

    // If the flight is "Delayed", the actual_departure cant't be before departure
    if (status == Delayed) {
        if (compare_datetimes(act_dep, dep) < 0) return false;
        if (compare_datetimes(act_arr, arr) < 0) return false;
    }

    // If the flight is "OnTime", the actuals must be >= than scheduled
    if (status == OnTime) {
        if (compare_datetimes(act_dep, dep) < 0) return false;
        if (compare_datetimes(act_arr, arr) < 0) return false;
    }

    // If it is "Cancelled", ignores (it's already validated in validate_status)
    return true;
}

// Aircraft of a flight must correspond to an existing aircraft.
bool validate_aircraft(const char *aircraft_id, AircraftsManager am) {
    if (!am || !aircraft_id || strlen(aircraft_id) == 0) return false;
    return aircrafts_manager_exists(am, aircraft_id);
}

// In case status of a flight is "Cancelled", actual departure and actual arrival must be "N/A".
bool validate_status(Flight f){
    if (!f) return false;

    flight_status status = get_flight_status(f);
    const char *ad = get_flight_actual_dep(f);
    const char *aa = get_flight_actual_ar(f);

    if (status == Cancelled) {
        if (!ad || !aa) return false;
        return strcmp(ad, "N/A") == 0 && strcmp(aa, "N/A") == 0;
    }
    return true;
}

// Flight ids of a reservation must correspond to a list of one or two existing flights.
bool validate_reservation_flights(Reservation r, Flight *flights, int num_flights, int num_ids) {
    if (!r || !flights || num_flights <= 0 || num_ids <= 0) return false;

    for (int i = 0; i < num_ids; i++) {
        const char *fid = get_reservation_flight_id(r, i);
        if (!fid) return false;
        bool found = false;
        for (int j = 0; j < num_flights && !found; j++) {
            if (strcmp(fid, get_flight_id(flights[j])) == 0)
                found = true;
        }
        if (!found) return false;
    }
    return true;
}

// Document number of a reservation must correspond to an existing passenger.
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

// Flight ids of a reservation must correspond to a list of one or two existing flights.
bool validate_reservation_connection(Reservation r, Flight *flights, int num_flights, int num_ids) {
    if (!r|| !flights) return false;
    if (num_ids != 2) return true; // only for reservations with 2 flights

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






