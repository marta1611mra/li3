#ifndef PARSER_RESERVATIONS_H
#define PARSER_RESERVATIONS_H

#include "dataset.h"
#include <glib.h>

/**
 * @brief Lê e processa o ficheiro de reservas, adicionando-as ao Dataset.
 *
 * @param d Dataset onde as reservas serão armazenadas.
 * @param data_path Caminho para a pasta ou ficheiro de dados de reservas.
 */
void parse_reservations(Dataset d, const char *data_path);
#endif