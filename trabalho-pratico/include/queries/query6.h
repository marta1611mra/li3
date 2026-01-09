#ifndef QUERY6_H
#define QUERY6_H

#include <stdio.h>
#include "dataset.h"
#include "managers/passengers_manager.h"
#include "managers/flights_manager.h"
#include "managers/reservations_manager.h"

/**
 * @brief Executa a Query 6.
 *
 * Identifica o aeroporto de destino mais frequente para passageiros
 * de uma determinada nacionalidade. Em caso de empate, escolhe o
 * aeroporto com código lexicograficamente menor.
 *
 * @param d Dataset que contém os dados necessários.
 * @param nationality Nacionalidade dos passageiros.
 * @param out Ficheiro de output onde o resultado será escrito.
 * @return void
 */
void q6(Dataset d,
        const char *nationality,
        FILE *out);

#endif
