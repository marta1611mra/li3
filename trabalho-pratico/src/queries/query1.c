#include <stdio.h>
#include <string.h>
#include <glib.h>
#include "query1.h"
#include "airports.h"
#include "airports_manager.h"


/**
 * Executa a Query 1: obter informação detalhada de um aeroporto específico.
 *
 * A query recebe um código de aeroporto (ex.: "LIS") e escreve no ficheiro
 * de saída uma linha com os seguintes campos, separados por vírgulas:
 *
 *     code,name,city,country,type
 *
 * Se o aeroporto não existir na estrutura, é escrita uma linha vazia.
 *
 * @param am   Estrutura AirportsManager contendo todos os aeroportos carregados.
 * @param code Código do aeroporto a procurar.
 * @param out  Ficheiro onde será escrita a resposta da query.
 *
 * @return Não retorna valor (void). Escreve diretamente no ficheiro de output.
 */

void query1(AirportsManager am, const char *code, FILE *out) {
    if (!am || !code || !out) return;

    Airport a = airports_manager_get(am, code);

    if(a) {
        fprintf(out, "%s,%s,%s,%s,%s\n", get_airport_code(a), get_airport_name(a), get_airport_city(a), get_airport_country(a), get_airport_type(a));
    }else{
        // Aeroporto não encontrado então imprime linha vazia
        fprintf(out,"\n");
    }
}