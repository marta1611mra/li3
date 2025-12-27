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
 * @param start_date  Data de início do intervalo (formato "YYYY-MM-DD").
 * @param end_date    Data de fim do intervalo (formato "YYYY-MM-DD").
 * @param flights     FlightsManager contendo todos os voos.
 * @param airports    AirportsManager contendo todos os aeroportos.
 * @param output      Ficheiro de output onde escrever o resultado.
 */

void q3(Dataset d, char **args, FILE *output);


#endif
