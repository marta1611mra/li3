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
 * @param start Data de início do período (inclusiva), como GDate*.
 * @param end Data de fim do período (inclusiva), como GDate*.
 * @param out   Ficheiro de output onde escrever o resultado 
 * 
 * Output: document_number;first_name;last_name;dob;nationality;count_top_10
 */

void query4_execute(Dataset d, const char *start, const char *end);
#endif

