#ifndef QUERY4_H
#define QUERY4_H

#include <stdio.h>
#include <glib.h>
#include "dataset.h"

/**
 * @brief Executa a Query 4.
 *
 * Calcula qual passageiro esteve mais vezes no Top 10 de passageiros
 * que mais gastaram semanalmente durante o intervalo [start, end].
 *
 * @param d Dataset com todos os dados necessários (reservas e passageiros).
 * @param begin_date Data de início do período (inclusiva), como GDate*.
 * @param end_date Data de fim do período (inclusiva), como GDate*.
 * @param out   Ficheiro de output onde escrever o resultado 
 * @return void
 * 
 */
void query4_execute(Dataset d, const char *begin_date, const char *end_date, FILE *out);
#endif

