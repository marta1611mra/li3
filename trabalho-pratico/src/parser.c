#include "parser.h"
#include "syntactic_validation.h"
#include "logical_validation.h"
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

struct dataset {
    GList *airports; 
    GList *flights;
    GList *aircrafts;
    GList *passengers;
    GList *reservations;
};

Dataset create_dataset() {
    Dataset d = malloc(sizeof(*d));
    if (!d) {
        fprintf(stderr, "ERROR: INSUFFICIENT MEMORY TO CREATE Dataset.\n");
        exit(EXIT_FAILURE);
    }

    d->airports     = NULL;
    d->flights      = NULL;
    d->aircrafts    = NULL;
    d->passengers   = NULL;
    d->reservations = NULL;
    return d;
}

void destroy_dataset(Dataset d) {
    if (!d) return;

    g_list_free_full(d->airports, (GDestroyNotify)destroy_airport);
}

