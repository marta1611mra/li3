#include <stdio.h>
#include <glib.h>
#include "flights.h"
#include "parser.c"

int main(){
    struct dataset d = {0};

    parse_flights(&d,"./dataset-fase-1");
    printf("Voos válidos:\n");
    for (GList *l = d.flights; l != NULL; l = l->next) {
        Flight f = l->data;
        printf("Flight %s: %s -> %s\n", f->flight_id, f->origin, f->destination);
    }

    g_list_free_full(d.flights, (GDestroyNotify)destroy_flight);
    g_list_free(d.aircrafts);

    return 0;
}