#ifndef QUERY2_H
#define QUERY2_H

#include <stdio.h>
#include "managers/flights_manager.h"
#include "managers/aircrafts_manager.h"

/**
 * @brief Executa a Query 2: lista as N aeronaves com mais voos.
 *
 * Conta o número de voos por aeronave (ignorando voos cancelados),
 * ordena as aeronaves por número de voos (descendente) e escreve as N
 * primeiras no ficheiro de output. Pode opcionalmente filtrar por fabricante.
 *
 * @param fm                FlightsManager contendo todos os voos.
 * @param am                AircraftsManager contendo todas as aeronaves.
 * @param N                 Número máximo de aeronaves a listar.
 * @param filter_manufacturer Se fornecido, apenas aeronaves do fabricante indicado são consideradas. Pode ser NULL.
 * @param out               Ficheiro de output onde escrever o resultado.
 */



void q2(FlightsManager fm, AircraftsManager am, int N, const char *filter_manufacturer, FILE *out);

#endif
