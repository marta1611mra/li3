#ifndef PARSER_FLIGHTS_H
#define PARSER_FLIGHTS_H

#include "dataset.h"
#include <glib.h>


/**
 * @brief Lê e processa o ficheiro de voos, adicionando-os ao Dataset.
 *
 * @param d Dataset onde os voos serão armazenados.
 * @param data_path Caminho para a pasta ou ficheiro de dados de voos.
 */
void parse_flights(Dataset d, const char *data_path);

#endif