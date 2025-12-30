#ifndef PARSER_H
#define PARSER_H

#include "dataset.h"
#include <glib.h>
#include <stdio.h>


/**
 * @brief Verifica se uma string contém apenas dígitos.
 * 
 * Percorre a string e verifica se todos os caracteres são numéricos.
 * 
 * @param s String a verificae.
 * @return true se a string contém apenas dígitos, false caso contrário ou se a string for NULL.
 */
bool is_all_digits(const char *s);

/**
 * @brief Remove aspas de uma string.
 *
 * Remove caracteres de aspas do início e do fim da string, modificando a string original.
 *
 * @param str String a processar.
 */
void remove_quotes(char *str);

/**
 * @brief Remove espaços em branco de uma string.
 *
 * Remove espaços e outros caracteres de espaçamento da string, modificando a string original.
 *
 * @param str String a processar.
 */
void remove_spc(char *str);

/**
 * @brief Imprime uma linha vazia para indicar ausência de resultados.
 *
 * Função auxiliar usada pelas queries para indicar que não há resultados a apresentar, escrevendo uma linha vazia no output.
 */
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
