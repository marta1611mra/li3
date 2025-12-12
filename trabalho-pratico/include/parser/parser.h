#ifndef PARSER_H
#define PARSER_H

#include "dataset.h"
#include <glib.h>
#include <stdio.h>


bool is_all_digits(const char *s);

void remove_quotes(char *str);


void remove_spc(char *str);


void exist_result(void);

/**
 * @brief Escreve o cabeçalho do ficheiro CSV de saída.
 * @param csv Ficheiro CSV de onde o cabeçalho será lido.
 * @param ferror Ficheiro onde o cabeçalho será escrito.
 */
void wcsv_header(FILE *csv, FILE *ferror);

/**
 * @brief Lê e processa todos os ficheiros de dados (airports, flights, aircrafts, passengers, reservations).
 *
 * @param d Dataset onde todos os dados serão armazenados.
 * @param data_path Caminho para a pasta que contém todos os ficheiros de dados.
 */
void parse_all(Dataset d, const char *data_path);

#endif
