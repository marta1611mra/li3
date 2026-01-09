#ifndef DATASET_H
#define DATASET_H
#include <glib.h>
#include "managers/airports_manager.h"
#include "managers/aircrafts_manager.h"
#include "managers/flights_manager.h"
#include "managers/passengers_manager.h"
#include "managers/reservations_manager.h"

/**
 * @struct dataset
 * @brief Estrutura opaca que representa o Dataset.
 *
 * A definição interna encontra-se em dataset.c.
 */
typedef struct dataset *Dataset;

/**
 * @brief Cria e inicializa um novo Dataset.
 *
 * Inicializa todos os managers e os índices auxiliares
 * necessários para as queries.
 *
 * @return Dataset inicializado ou NULL em caso de erro.
 */
Dataset dataset_create(void);

/**
 * @brief Liberta toda a memória associada ao Dataset.
 *
 * Destroi todos os managers internos, índices auxiliares
 * e a própria estrutura Dataset.
 *
 * @param d Dataset a destruir.
 */
void dataset_destroy(Dataset d);

/**
 * @brief Obtém o manager de aeroportos.
 * @param d Dataset.
 * @return AirportsManager associado.
 */
AirportsManager dataset_get_airports(Dataset d);

/**
 * @brief Obtém o manager de aeronaves.
 * @param d Dataset.
 * @return AircraftsManager associado.
 */
AircraftsManager dataset_get_aircrafts(Dataset d);

/**
 * @brief Obtém o manager de voos.
 * @param d Dataset.
 * @return FlightsManager associado.
 */
FlightsManager dataset_get_flights(Dataset d);

/**
 * @brief Obtém o manager de passageiros.
 * @param d Dataset.
 * @return PassengersManager associado.
 */
PassengersManager dataset_get_passengers(Dataset d);

/**
 * @brief Obtém o manager de reservas.
 * @param d Dataset.
 * @return ReservationsManager associado.
 */
ReservationsManager dataset_get_reservations(Dataset d);

/**
 * @brief Atualiza o índice da Query 2.
 *
 * Estrutura:
 *   manufacturer -> (aircraft_id -> count)
 *
 * Existe também uma entrada especial "__ALL__"
 * para o total global.
 *
 * @param d Dataset.
 * @param aircraft_id Identificador da aeronave.
 * @param manufacturer Fabricante da aeronave.
 */
void dataset_update_q2(Dataset d,
                       const char *aircraft_id,
                       const char *manufacturer);

/**
 * @brief Obtém o índice da Query 2.
 *
 * @param d Dataset.
 * @return GHashTable do índice da Query 2.
 */
GHashTable *dataset_get_q2_index(Dataset d);

/**
 * @brief Atualiza o índice da Query 3.
 *
 * Estrutura:
 *   airport_code -> (date -> count)
 *
 * A data é considerada apenas no formato YYYY-MM-DD,
 * permitindo comparações lexicográficas diretas.
 *
 * @param d Dataset.
 * @param airport_code Código do aeroporto de origem.
 * @param date Data de partida (YYYY-MM-DD ou com timestamp).
 */
void dataset_update_q3(Dataset d,
                       const char *airport_code,
                       const char *date);

/**
 * @brief Obtém o índice da Query 3.
 *
 * @param d Dataset.
 * @return GHashTable do índice da Query 3.
 */
GHashTable *dataset_get_q3_index(Dataset d);

/**
 * @brief Adiciona dados agregados para a Query 4.
 *
 * Estrutura:
 *   week_sunday -> (document -> total_price)
 *
 * A semana (domingo) é calculada apenas uma vez
 * durante o parsing, eliminando trabalho na query.
 *
 * @param d Dataset.
 * @param document Documento do passageiro.
 * @param price Preço da reserva.
 * @param date Data da reserva (YYYY-MM-DD).
 */
void dataset_add_q4_data(Dataset d,
                         const char *document,
                         double price,
                         const char *date);

/**
 * @brief Obtém o índice agregado da Query 4.
 *
 * @param d Dataset.
 * @return GHashTable com os dados da Query 4.
 */
GHashTable *dataset_get_q4_weeks(Dataset d);

/**
 * @brief Atualiza o índice da Query 6.
 *
 * Estrutura:
 *   nationality -> (airport -> count)
 *
 * @param d Dataset.
 * @param nationality Nacionalidade do passageiro.
 * @param airport Aeroporto de destino.
 */
void dataset_update_q6(Dataset d,
                       const char *nationality,
                       const char *airport);

/**
 * @brief Constrói o índice da Query 6.
 *
 * O índice é construído incrementalmente durante o parsing,
 * pelo que esta função existe apenas para compatibilidade.
 *
 * @param d Dataset.
 */
void dataset_build_q6_index(Dataset d);

/**
 * @brief Obtém o índice da Query 6.
 *
 * @param d Dataset.
 * @return GHashTable do índice da Query 6.
 */
GHashTable *dataset_get_q6_index(Dataset d);

/**
 * @brief Atualiza os dados da Query 5 durante o parsing de voos.
 *
 * Calcula o atraso de um voo e acumula nas estatísticas da companhia aérea. 
 * Apenas atrasos positivos são contabilizados.
 *
 * @param d Dataset onde os dados serão armazenados.
 * @param airline Nome da companhia aérea.
 * @param sched_dep Data e hora programada de partida (formato: "YYYY-MM-DD HH:MM").
 * @param real_dep Data e hora real de partida (formato: "YYYY-MM-DD HH:MM").
 */
void dataset_update_q5(Dataset d, char *airline, char *sched_dep, char *real_dep);

/**
 * @brief Finaliza o processamento dos dados da Query 5.
 *
 * Converte os dados temporários num array ordenado, calcula médias e ordena as companhias por atraso médio decrescente.
 * É chamada após todo o parsing estar completo.
 *
 * @param d Dataset a finalizar.
 */
void dataset_finalize_q5(Dataset d);

/**
 * @brief Obtém o número total de companhias aéreas nos dados da Query 5.
 *
 * @param d Dataset.
 * @return Número de companhias aéreas processadas.
 */
int dataset_q5_get_count(Dataset d);

/**
 * @brief Obtém o nome de uma companhia aérea na posição em questão.
 *
 * Acede ao array ordenado da Query 5 e retorna o nome da companhia na posição index.
 *
 * @param d Dataset.
 * @param index Índice da companhia no array ordenado.
 * @return Nome da companhia aérea, ou NULL se index inválido.
 */
char *dataset_q5_get_name(Dataset d, int index);

/**
 * @brief Obtém o número de voos de uma companhia aérea na posição em questão.
 *
 * Acede ao array ordenado da Query 5 e retorna o total de voos com atraso da companhia na posição index.
 *
 * @param d Dataset.
 * @param index Índice da companhia no array ordenado.
 * @return Número de voos com atraso, ou 0 se index inválido.
 */
int dataset_q5_get_flights_count(Dataset d, int index);

/**
 * @brief Obtém o atraso médio de uma companhia aérea na posição especificada.
 *
 * Acede ao array ordenado da Query 5 e retorna o atraso médio (em minutos) da companhia na posição index.
 *
 * @param d Dataset.
 * @param index Índice da companhia no array ordenado.
 * @return Atraso médio em minutos (arredondado a 3 casas decimais), ou 0.0 se index inválido.
 */
double dataset_q5_get_average(Dataset d, int index);

#endif 