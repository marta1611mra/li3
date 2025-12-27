#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include "query6.h"
#include "entities/reservations.h"
#include "entities/passengers.h"
#include "entities/flights.h"
#include "output_format.h"


typedef struct {
    const char *best_code;
    int best_count;
} BestAirport;

typedef struct {
    GHashTable *arrivals;        // airport_code -> int*
    PassengersManager pm;
    FlightsManager fm;
    const char *nationality;
} Q6Ctx;

/* Escolhe o voo final com base na data de chegada */
static Flight get_final_flight(FlightsManager fm, Reservation r) {
    const char *fid0 = get_reservation_flight_id(r, 0);
    const char *fid1 = get_reservation_flight_id(r, 1);

    Flight f0 = fid0 ? flights_manager_get(fm, fid0) : NULL;
    Flight f1 = fid1 ? flights_manager_get(fm, fid1) : NULL;

    if (!f0 && !f1) return NULL;
    if (!f0) return f1;
    if (!f1) return f0;

    return strcmp(
        get_flight_arrival(f0),
        get_flight_arrival(f1)
    ) >= 0 ? f0 : f1;
}



/* Incrementa o contador de um aeroporto na hash table */
static void increment_airport(GHashTable *arrivals, const char *airport) {
    int *count = g_hash_table_lookup(arrivals, airport);

    if (!count) {
        count = malloc(sizeof(int));
        *count = 0;
        g_hash_table_insert(arrivals, g_strdup(airport), count);
    }

    (*count)++;
}

static void count_arrivals(gpointer value, gpointer user_data) {
    Reservation r = value;
    Q6Ctx *ctx = user_data;

    const char *doc = get_reservation_document_number(r);
    if (!doc) return;

    Passenger p = passengers_manager_get(ctx->pm, doc);
    if (!p) return;

    if (strcmp(get_passenger_nationality(p), ctx->nationality) != 0)
        return;

    Flight final = get_final_flight(ctx->fm, r);
    if (!final) return;

    const char *airport = get_flight_dest(final);
    if (!airport) return;

    increment_airport(ctx->arrivals, airport);
}


static void select_best_airport(gpointer key, gpointer value, gpointer user_data) {
    const char *airport = key;
    int count = *(int *)value;
    BestAirport *best = user_data;

    if (!best->best_code ||
        count > best->best_count ||
        (count == best->best_count &&
         strcmp(airport, best->best_code) < 0)) {

        best->best_code = airport;
        best->best_count = count;
    }
}

void q6(PassengersManager pm,
            FlightsManager fm,
            ReservationsManager rm,
            const char *nationality,
            FILE *out) {

    if (!pm || !fm || !rm || !nationality || !out) {
        fprintf(out, "\n");
        return;
    }

    GHashTable *arrivals =
        g_hash_table_new_full(g_str_hash, g_str_equal, free, free);

    Q6Ctx ctx = { arrivals, pm, fm, nationality };

    GList *list = reservations_manager_get_all(rm);

    for (GList *l = list; l != NULL; l = l->next) {
    Reservation r = l->data;
    count_arrivals(r, &ctx);
    }

    g_list_free(list);


    BestAirport best = { NULL, 0 };
    g_hash_table_foreach(arrivals, select_best_airport, &best);

    if (best.best_code) {
        fprintf(out, "%s;%d\n", best.best_code, best.best_count);
    } else {
        fprintf(out, "\n");
    }

    g_hash_table_destroy(arrivals);
}

