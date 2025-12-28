#define _XOPEN_SOURCE
#include "query5.h"
#include "dataset.h"
#include "flights_manager.h"
#include "flights.h"
#include "output_format.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <glib.h>


void query5(Dataset d, int N, FILE *out) {
    if (!d || !out) return;

    // Pedimos quantos existem no total
    int total_airlines = dataset_q5_get_count(d);

    char sep = get_output_separator();

    // Loop simples usando os getters
    for (int i = 0; i < N && i < total_airlines; i++) {
        fprintf(out, "%s%c%d%c%.3f\n", 
            dataset_q5_get_name(d, i),          // Getter do nome
            sep,
            dataset_q5_get_flights_count(d, i), // Getter do total
            sep,
            dataset_q5_get_average(d, i)        // Getter da média
        );
    }
}