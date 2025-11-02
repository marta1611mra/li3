#include <stdio.h>
#include <glib.h>
#include "parser.h"
#include "dataset.h"
#include "entities/flights.h"

int main() {
    Dataset d = create_dataset(); // função do dataset.c

    parse_flights(d, "./dataset-fase-1");

    FlightsManager fm = dataset_get_flights(d);
    // Supondo que tens um iterador ou função para listar voos:
    // ex: flights_manager_print_all(fm);

    destroy_dataset(d); // liberta tudo
    return 0;
}
