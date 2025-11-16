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

/**
 * @brief Garante que a pasta "resultados" existe.
 *
 * Se a pasta ainda não existir, cria usando comando POSIX `mkdir -p`.
 */
static void ensure_result_dir(void) {
    system("mkdir -p resultados");
}

/**
 * @brief Função principal do programa.
 *
 * Fluxo de execução:
 * 1. Verifica os argumentos de linha de comando.
 * 2. Cria a estrutura principal do dataset.
 * 3. Carrega os dados de aeroportos, aeronaves, voos, passageiros e reservas.
 * 4. Garante que a pasta "resultados" existe.
 * 5. Processa as queries definidas no ficheiro de comandos.
 * 6. Liberta a memória alocada para o dataset.
 *
 * Uso:
 * ./programa <pasta_dataset> <ficheiro_comandos>
 *
 * @param argc Número de argumentos da linha de comando.
 * @param argv Array de strings com os argumentos da linha de comando.
 * @return 0 se execução bem sucedida, 1 em caso de erro.
 */
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
    parse_airports(d, dataset_path);
    parse_aircrafts(d, dataset_path);
    parse_flights(d, dataset_path);
    parse_passengers(d, dataset_path);
    parse_reservations(d, dataset_path);

    // Garante que a pasta de resultados existe
    ensure_result_dir();

    // Processa as queries a partir do ficheiro de comandos
    process_queries(d, commands_file);

    // Liberta memória alocada para o dataset
    dataset_destroy(d);

    return 0;
}
