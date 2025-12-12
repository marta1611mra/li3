#ifndef PARSER_AIRCRAFTS_H
#define PARSER_AIRCRAFTS_H

#include "dataset.h"
#include <glib.h>


/**
 * @brief Lê e processa o ficheiro de aeronaves, adicionando-as ao Dataset.
 *
 * @param d Dataset onde as aeronaves serão armazenadas.
 * @param data_path Caminho para a pasta ou ficheiro de dados de aeronaves.
 */
void parse_aircrafts(Dataset d, const char *data_path);

#endif