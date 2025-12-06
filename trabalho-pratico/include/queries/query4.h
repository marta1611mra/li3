#ifndef QUERY4_H
#define QUERY4_H

#include <stdio.h>
#include "date.h"
#include "dataset.h"

/**
 * @brief Executa a Query 4.
 *
 * Calcula qual passageiro esteve mais vezes no Top 10 de passageiros
 * que mais gastaram semanalmente durante o intervalo [start, end].
 *
 * Para cada semana:
 * 1. Agrupa as reservas dos passageiros.
 * 2. Calcula o total gasto por passageiro.
 * 3. Determina o Top 10 de gastos da semana.
 * 4. Incrementa o contador de semanas em que cada passageiro aparece no Top 10.
 *
 * No final, seleciona o passageiro com mais semanas no Top 10 e imprime os dados.
 *
 * @param d Dataset com todos os dados necessários (reservas e passageiros).
 * @param start Data inicial do período a considerar.
 * @param end   Data final do período a considerar.
 * @param out   Ficheiro de output onde escrever o resultado (ex: stdout ou ficheiro).
 */

void query4(Dataset d, Date start, Date end, FILE *out);

#endif

