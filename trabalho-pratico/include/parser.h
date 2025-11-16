#ifndef PARSER_H
#define PARSER_H

#include "dataset.h"
#include <glib.h>

/**
 * @brief Lê e processa o ficheiro de aeroportos, adicionando-os ao Dataset.
 *
 * @param d Dataset onde os aeroportos serão armazenados.
 * @param data_path Caminho para a pasta ou ficheiro de dados de aeroportos.
 */
void parse_airports(Dataset d, const char *data_path);

/**
 * @brief Lê e processa o ficheiro de voos, adicionando-os ao Dataset.
 *
 * @param d Dataset onde os voos serão armazenados.
 * @param data_path Caminho para a pasta ou ficheiro de dados de voos.
 */
void parse_flights(Dataset d, const char *data_path);

/**
 * @brief Lê e processa o ficheiro de aeronaves, adicionando-as ao Dataset.
 *
 * @param d Dataset onde as aeronaves serão armazenadas.
 * @param data_path Caminho para a pasta ou ficheiro de dados de aeronaves.
 */
void parse_aircrafts(Dataset d, const char *data_path);

/**
 * @brief Lê e processa o ficheiro de passageiros, adicionando-os ao Dataset.
 *
 * @param d Dataset onde os passageiros serão armazenados.
 * @param data_path Caminho para a pasta ou ficheiro de dados de passageiros.
 */
void parse_passengers(Dataset d, const char *data_path);

/**
 * @brief Lê e processa o ficheiro de reservas, adicionando-as ao Dataset.
 *
 * @param d Dataset onde as reservas serão armazenadas.
 * @param data_path Caminho para a pasta ou ficheiro de dados de reservas.
 */
void parse_reservations(Dataset d, const char *data_path);

/**
 * @brief Lê e processa todos os ficheiros de dados (airports, flights, aircrafts, passengers, reservations).
 *
 * @param d Dataset onde todos os dados serão armazenados.
 * @param data_path Caminho para a pasta que contém todos os ficheiros de dados.
 */
void parse_all(Dataset d, const char *data_path);

#endif
