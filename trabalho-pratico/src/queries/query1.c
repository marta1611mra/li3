/**
 * @file query1.c
 * @brief Implementação da Query 1: informação detalhada de um aeroporto.
 *
 * Este módulo fornece a função para executar a Query 1, que dado um código
 * de aeroporto (IATA), retorna informações detalhadas do aeroporto, incluindo
 * nome, cidade, país e tipo, e escreve os resultados num ficheiro de output.
 */

#include <stdio.h>
#include <string.h>
#include <glib.h>
#include "query1.h"
#include "airports.h"
#include "airports_manager.h"

/**
 * @brief Executa a Query 1.
 *
 * A Query 1 recebe um código de aeroporto e escreve no ficheiro de saída
 * uma linha com os seguintes campos, separados por vírgulas:
 *
 *      code,name,city,country,type
 *
 * Se o aeroporto não existir na estrutura, escreve uma linha vazia.
 *
 * @param am Estrutura AirportsManager contendo todos os aeroportos carregados.
 * @param code Código do aeroporto a procurar (ex.: "LIS").
 * @param out Ficheiro onde será escrita a resposta da query.
 *
 * @note A função não retorna valor. Toda a saída é escrita diretamente no ficheiro.
 * @note Se algum parâmetro for NULL, a função não realiza qualquer ação.
 */
void query1(AirportsManager am, const char *code, FILE *out) {
    if (!am || !code || !out) return;

    Airport a = airports_manager_get(am, code);

    if(a) {
        fprintf(out, "%s,%s,%s,%s,%s\n",
                get_airport_code(a),
                get_airport_name(a),
                get_airport_city(a),
                get_airport_country(a),
                get_airport_type(a));
    } else {
        // Aeroporto não encontrado: imprime linha vazia
        fprintf(out, "\n");
    }
}
