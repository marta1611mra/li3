#ifndef PARSER_PASSANGERS_H
#define PARSER_PASSANGERS_H

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