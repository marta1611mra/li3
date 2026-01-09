#ifndef QUERY3_H
#define QUERY3_H

#include <stdio.h>
#include <stdbool.h>
#include "dataset.h"
#include "flights_manager.h"
#include "airports_manager.h"
#include "flights.h"
#include "airports.h"

/**
 * @brief Executa a Query 3: encontra o aeroporto com mais partidas num intervalo de datas.
 *
 * Conta o número de partidas reais de cada aeroporto dentro do intervalo de datas
 * fornecido e escreve no ficheiro de output o aeroporto com mais partidas.
 * Em caso de empate, o aeroporto com o código lexicograficamente menor é escolhido.
 *
 * @param d O dataset contendo os dados de voos e aeroportos.
 * @param args Array de strings contendo as datas de início e fim do intervalo (YYYY-MM
 * @param output Ficheiro onde o resultado será escrito.
 * @return void
 */
void q3(Dataset d, char *args[], FILE *output);

#endif
