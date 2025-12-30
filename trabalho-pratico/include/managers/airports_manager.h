#ifndef AIRPORTS_MANAGER_H
#define AIRPORTS_MANAGER_H
#include "airports.h"
#include <glib.h>

/**
 * @brief Opaque pointer para a estrutura AirportsManager.
 *
 * Gerencia todos os aeroportos do sistema.
 */
typedef struct airports_manager *AirportsManager;

/**
 * @brief Cria um novo AirportsManager.
 *
 * Inicializa a estrutura que irá armazenar todos os aeroportos.
 *
 * @return Ponteiro para o novo AirportsManager criado. NULL se falhar a alocação.
 */
AirportsManager airports_manager_create();

/**
 * @brief Liberta a memória associada ao AirportsManager.
 *
 * @param m AirportsManager a destruir. Ignorado se for NULL.
 */
void airports_manager_destroy(AirportsManager m);

/**
 * @brief Adiciona um aeroporto ao AirportsManager.
 *
 * @param m AirportsManager.
 * @param a Airport a adicionar.
 */
void airports_manager_add(AirportsManager m, Airport a);

/**
 * @brief Obtém um aeroporto pelo seu código IATA.
 *
 * @param m AirportsManager.
 * @param code Código IATA do aeroporto a procurar.
 * @return Ponteiro para o aeroporto encontrado ou NULL se não existir.
 */
Airport airports_manager_get(AirportsManager m, const char *code);

/**
 * @brief Obtém o número total de aeroportos armazenados.
 *
 * @param m AirportsManager.
 * @return Número de aeroportos.
 */
int airports_manager_count(AirportsManager m);

/**
 * @brief Regista uma partida no aeroporto em questão.
 * 
 * Incrementa o contador de partidas do aeroporto identificado pelo código.
 * Usado durante o parsing para juntar estatísticas.
 * 
 * @param a AirportsManager.
 * @param code Código do aeroporto.
 * @param passengers Número de passageiros na partida.
 */
void airports_manager_departure(AirportsManager m, const char *code, int passengers);

/**
 * @brief Regista uma chegada no aeroporto em questão.
 * 
 * Incrementa o contador de chegadas do aeroporto identificado pelo código.
 * Usado durante o parsing para juntar estatísticas.
 * 
 * @param a AirportsManager.
 * @param code Código do aeroporto.
 * @param passengers Número de passageiros na chegada.
 */
void airports_manager_arrival(AirportsManager m, const char *code, int passengers);

/**
 * @brief Obtém o número total de chegadas a um aeroporto.
 * 
 * @param a AirportsManager.
 * @param code Código do aeroporto.
 * @param passengers Número total de passageiros em chegadas, ou 0 se o aeroporto não existir.
 */
int airports_manager_get_arrivals(AirportsManager m, const char *code);

/**
 * @brief Obtém o número total de partidas de um aeroporto.
 * 
 * @param a AirportsManager.
 * @param code Código do aeroporto.
 * @param passengers Número total de passageiros em partidas, ou 0 se o aeroporto não existir.
 */
int airports_manager_get_departures(AirportsManager m, const char *code);

#endif
