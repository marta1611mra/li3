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

/* Structs de RESULTADO → fazem parte da API */
typedef struct {
    char id[16];
    char manufacturer[64];
    char model[64];
    char count_str[16];
} Q2Row;

typedef struct {
    Q2Row *rows;
    int used;
} Q2Result;


Q2Result q2(FlightsManager fm, AircraftsManager am, int N, const char *filter_manufacturer);
void free_q2_result(Q2Result *r);

#endif
