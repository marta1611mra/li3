#ifndef AIRPORTS_H
#define AIRPORTS_H

/**
 * @brief Opaque pointer para a estrutura Airport.
 *
 * Representa um aeroporto do sistema.
 */

typedef struct airport *Airport;


/**
 * @brief Cria um novo aeroporto.
 *
 * Inicializa um objeto Airport com todos os campos fornecidos.
 *
 * @param code      Código IATA do aeroporto (ex.: "LIS").
 * @param name      Nome completo do aeroporto.
 * @param city      Cidade onde o aeroporto se encontra.
 * @param country   País do aeroporto.
 * @param latitude  Latitude geográfica do aeroporto.
 * @param longitude Longitude geográfica do aeroporto.
 * @param icao      Código ICAO do aeroporto (opcional, pode ser NULL).
 * @param type      Tipo de aeroporto (ex.: "International", "Domestic").
 * @return Ponteiro para o novo Airport criado. NULL se falhar a alocação.
 */

Airport create_airport(const char *code, const char *name, const char *city, const char *country, double latitude, double longitude, const char *icao, const char *type);

/**
 * @brief Liberta a memória associada a um aeroporto.
 *
 * @param a Airport a destruir. Ignorado se for NULL.
 */
void destroy_airport(Airport a);


/**
 * @brief Obtém o código IATA do aeroporto.
 *
 * @param a Airport.
 * @return String com o código IATA.
 */
const char *get_airport_code(Airport a);

/**
 * @brief Obtém o nome completo do aeroporto.
 *
 * @param a Airport.
 * @return String com o nome do aeroporto.
 */
const char *get_airport_name(Airport a);

/**
 * @brief Obtém a cidade do aeroporto.
 *
 * @param a Airport.
 * @return String com o nome da cidade.
 */
const char *get_airport_city(Airport a);

/**
 * @brief Obtém o país do aeroporto.
 *
 * @param a Airport.
 * @return String com o nome do país.
 */
const char *get_airport_country(Airport a);


/**
 * @brief Obtém o tipo do aeroporto.
 *
 * @param a Airport.
 * @return String com o tipo do aeroporto (ex.: "International", "Domestic").
 */
const char *get_airport_type(Airport a);

void airport_add_arrival_passengers(Airport a, int count);
void airport_add_departure_passengers(Airport a, int count);
int get_airport_arrivals_count(Airport a);
int get_airport_departures_count(Airport a);

#endif