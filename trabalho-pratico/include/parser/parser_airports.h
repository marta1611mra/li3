#ifndef PARSER_AIRPORTS_H
#define PARSER_AIRPORTS_H

#include "dataset.h"
#include <glib.h>

/**
 * @brief Lê e processa o ficheiro de aeroportos, adicionando-os ao Dataset.
 *
 * @param d Dataset onde os aeroportos serão armazenados.
 * @param data_path Caminho para a pasta ou ficheiro de dados de aeroportos.
 */
void parse_airports(Dataset d, const char *data_path);

#endif