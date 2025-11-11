#include <stdio.h>
#include <glib.h>
#include "dataset.h"
#include "parser.h"
#include "entities/flights.h"
#include "entities/reservations.h"
#include "entities/passengers.h"
#include "entities/aircrafts.h"
#include "entities/airports.h"
#include "managers/flights_manager.h"
#include "managers/reservations_manager.h"
#include "managers/passengers_manager.h"
#include "managers/aircrafts_manager.h"
#include "managers/airports_manager.h"
#include "programa-principal.h"
#include "query2.h"
#include "query3.h"
#include "query1.h"
#include "queries.h"

// Função auxiliar para garantir que a pasta resultados existe
static void ensure_result_dir(void) {
    system("mkdir -p resultados");
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <pasta_dataset> <ficheiro_comandos>\n", argv[0]);
        return 1;
    }
    
    const char *dataset_path = argv[1];
    const char *commands_file = argv[2];

    Dataset d = dataset_create();
    if (!d) {
        fprintf(stderr, "Erro ao criar estrutura do dataset.\n");
        return 1;
    }

    printf("A carregar datasets...\n");
    parse_airports(d, dataset_path);
    parse_aircrafts(d, dataset_path);
    parse_flights(d, dataset_path);
    parse_passengers(d, dataset_path);
    parse_reservations(d, dataset_path);

    ensure_result_dir();

    process_queries(d, commands_file);

    dataset_destroy(d);
    return 0;
}
