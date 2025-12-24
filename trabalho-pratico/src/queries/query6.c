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
    GHashTable *arrivals;        // airport_code -> int*
    PassengersManager pm;
    FlightsManager fm;
    const char *nationality;
} Q6Ctx;

static void count_arrivals(gpointer value, gpointer user_data) {
    Reservation r = value;
    Q6Ctx *ctx = user_data;

    const char *doc = get_reservation_document_number(r);
    if (!doc) return;

    Passenger p = passengers_manager_get(ctx->pm, doc);
    if (!p) return;

    if (strcmp(get_passenger_nationality(p), ctx->nationality) != 0)
        return;

    for (int i = 0; i < 2; i++) {
        const char *flight_id = get_reservation_flight_id(r, i);
        if (!flight_id || flight_id[0] == '\0')
            continue;

        Flight f = flights_manager_get(ctx->fm, flight_id);
        if (!f) continue;

        if (get_flight_status(f) == Cancelled)
            continue;

        const char *dest = get_flight_dest(f);
        if (!dest) continue;

        int *cnt = g_hash_table_lookup(ctx->arrivals, dest);
        if (cnt) {
            (*cnt)++;
        } else {
            int *c = malloc(sizeof(int));
            *c = 1;
            g_hash_table_insert(ctx->arrivals, g_strdup(dest), c);
        }
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

    GList *reservations = reservations_manager_get_all(rm);
    for (GList *l = reservations; l; l = l->next) {
        count_arrivals(l->data, &ctx);
    }

    char best[4] = "";
    int max = 0;

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, arrivals);

    while (g_hash_table_iter_next(&iter, &key, &value)) {
        char *code = key;
        int count = *(int *)value;

        if (count > max ||
            (count == max &&
             (best[0] == '\0' || strcmp(code, best) < 0))) {
            strncpy(best, code, 3);
            best[3] = '\0';
            max = count;
        }
    }

    if (best[0] == '\0') {
        fprintf(out, "\n");
    } else {
        char sep = get_output_separator();
        fprintf(out, "%s%c%d\n", best, sep, max);
    }

    g_hash_table_destroy(arrivals);
}
