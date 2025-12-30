#ifndef QUERY1_H
#define QUERY1_H

#include "dataset.h"
#include <stdio.h>

/**
 * @brief Executa a Query 1: pesquisa um aeroporto pelo código.
 *
 * Procura um aeroporto no AirportsManager pelo código IATA fornecido
 * e escreve os detalhes do aeroporto no ficheiro de output.
 *
 * @param am  AirportsManager onde procurar o aeroporto.
 * @param code Código IATA do aeroporto a pesquisar.
 * @param out Ficheiro de output onde escrever o resultado.
 */
void q1(AirportsManager am, const char *code,FILE *out);

#endif
