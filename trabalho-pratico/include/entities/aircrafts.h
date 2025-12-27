#ifndef AIRCRAFTS_H
#define AIRCRAFTS_H


/**
 * @brief Opaque pointer para a estrutura Aircraft.
 *
 * Representa uma aeronave do sistema.
 */

typedef struct aircraft *Aircraft;


/**
 * @brief Cria uma nova aeronave.
 *
 * Inicializa um objeto Aircraft com todos os campos fornecidos.
 *
 * @param id           Identificador único da aeronave.
 * @param manufacturer Fabricante da aeronave.
 * @param model        Modelo da aeronave.
 * @param year         Ano de fabrico da aeronave.
 * @param capacity     Capacidade máxima de passageiros.
 * @param range        Alcance máximo em km.
 * @return Ponteiro para o novo Aircraft criado. NULL se falhar a alocação.
 */

Aircraft create_aircraft(const char *id, const char *manufacturer, const char *model, int year, int capacity, int range);

/**
 * @brief Liberta a memória associada a uma aeronave.
 *
 * @param a Aircraft a destruir. Ignorado se for NULL.
 */
void destroy_aircraft(Aircraft a);

/**
 * @brief Obtém o identificador da aeronave.
 *
 * @param a Aircraft.
 * @return String com o ID da aeronave.
 */
const char *get_aircraft_id(Aircraft a);

/**
 * @brief Obtém o fabricante da aeronave.
 *
 * @param a Aircraft.
 * @return String com o nome do fabricante.
 */
const char *get_aircraft_manufacturer(Aircraft a);

/**
 * @brief Obtém o modelo da aeronave.
 *
 * @param a Aircraft.
 * @return String com o modelo da aeronave.
 */
const char *get_aircraft_model(Aircraft a);



#endif
