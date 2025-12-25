#ifndef PARSER_PASSENGERS_H
#define PARSER_PASSENGERS_H

#include "dataset.h"
#include <glib.h>


/**
 * @brief Lê e processa o ficheiro de passageiros, adicionando-os ao Dataset.
 *
 * @param d Dataset onde os passageiros serão armazenados.
 * @param data_path Caminho para a pasta ou ficheiro de dados de passageiros.
 */
void parse_passengers(Dataset d, const char *data_path);

#endif