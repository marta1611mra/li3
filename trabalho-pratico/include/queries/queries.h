#ifndef QUERIES_H
#define QUERIES_H
#include "dataset.h"

/**
 * @brief Processa todas as queries listadas num ficheiro de input.
 *
 * Esta função lê cada linha do ficheiro especificado em `queries_path`,
 * identifica a query a executar e cria um ficheiro de output correspondente
 * para cada query (ex.: command1_output.txt, command2_output.txt, ...).
 *
 * @param d Dataset que contém todos os managers (airports, aircrafts, flights, passengers, reservations).
 * @param queries_path Caminho para o ficheiro de input contendo as queries.
 */
void process_queries(Dataset d, const char *queries_path);
#endif
