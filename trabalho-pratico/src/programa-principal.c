#include <stdio.h>
#include <glib.h>
#include "dataset.h"
#include "parser/parser.h"
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


// Garante que a pasta "resultados" existe.
static void ensure_result_dir(void) {
    system("mkdir -p resultados");
}


// Função principal do programa.
int main(int argc, char *argv[]) {
    // Verifica se os argumentos foram fornecidos corretamente
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <pasta_dataset> <ficheiro_comandos>\n", argv[0]);
        return 1;
    }
    
    const char *dataset_path = argv[1];     // Caminho para a pasta com os datasets
    const char *commands_file = argv[2];    // Caminho para o ficheiro de comandos

    // Cria a estrutura do dataset
    Dataset d = dataset_create();
    if (!d) {
        fprintf(stderr, "Erro ao criar estrutura do dataset.\n");
        return 1;
    }

    printf("A carregar datasets...\n");

    // Carrega os diferentes tipos de dados para o dataset
    parse_all (d, dataset_path);

    // Garante que a pasta de resultados existe
    ensure_result_dir();

    // Processa as queries a partir do ficheiro de comandos
    process_queries(d, commands_file);

    // Liberta memória alocada para o dataset
    dataset_destroy(d);

    return 0;
}
