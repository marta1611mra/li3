#ifndef QUERY5_H
#define QUERY5_H
#include <stdio.h>
#include "dataset.h"

/**
 * @brief Executa a Query 5.
 *
 * Lista as N companhias aéreas com maior atraso médio nas partidas,
 * ordenadas por atraso médio decrescente. Em caso de empate, ordena
 * lexicograficamente pelo nome da companhia.
 *
 * @param d Dataset que contém os dados necessários.
 * @param N Número de companhias a listar.
 * @param out Ficheiro de output onde os resultados serão escritos.
 * @return void
 */
void query5(Dataset d, int N, FILE *out);

#endif
