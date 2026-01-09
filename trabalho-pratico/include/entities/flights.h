#ifndef FLIGHTS_H
#define FLIGHTS_H


/**
 * @brief Opaque pointer para a estrutura Flight.
 *
 * Representa um voo do sistema.
 */
typedef struct flight *Flight;

/**
 * @brief Enumeração para o estado de um voo.
 */
typedef enum {
    OnTime,          /** Voo no horário previsto */
    Delayed,         /** Voo atrasado */
    Cancelled        /** Voo cancelado */
} flight_status;


/**
 * @brief Cria um novo voo.
 *
 * Inicializa um objeto Flight com todos os campos fornecidos.
 *
 * @param flight_id        Identificador único do voo.
 * @param departure        Hora de partida planeada (YYYY-MM-DD HH:MM).
 * @param actual_departure Hora de partida real (YYYY-MM-DD HH:MM ou "N/A").
 * @param arrival          Hora de chegada planeada (YYYY-MM-DD HH:MM).
 * @param actual_arrival   Hora de chegada real (YYYY-MM-DD HH:MM ou "N/A").
 * @param gate             Portão de embarque.
 * @param status           Estado do voo (OnTime, Delayed, Cancelled).
 * @param origin           Código do aeroporto de origem.
 * @param destination      Código do aeroporto de destino.
 * @param aircraft_id      ID da aeronave utilizada no voo.
 * @param airline          Companhia aérea.
 * @param tracking_url     URL para rastreio do voo (opcional, pode ser NULL).
 * @return Ponteiro para o novo Flight criado. NULL se falhar a alocação.
 */
Flight create_flight(const char *flight_id, const char *departure, const char *actual_departure, const char *arrival, const char *actual_arrival, const char *gate, flight_status status, const char *origin, const char *destination, const char *aircraft_id, const char *airline, const char *tracking_url);


/**
 * @brief Liberta a memória associada a um voo.
 *
 * @param f Flight a destruir. Ignorado se for NULL.
 */
void destroy_flight(Flight f);


/**
 * @brief Obtém o aeroporto de origem do voo.
 *
 * @param f Flight.
 * @return Código do aeroporto de origem.
 */
const char *get_flight_orig(Flight f);

/**
 * @brief Obtém o aeroporto de destino do voo.
 *
 * @param f Flight.
 * @return Código do aeroporto de destino.
 */
const char *get_flight_dest(Flight f);

/**
 * @brief Obtém o estado do voo.
 *
 * @param f Flight.
 * @return Estado do voo (OnTime, Delayed, Cancelled).
 */
flight_status get_flight_status(Flight f);


/**
 * @brief Obtém o identificador do voo.
 *
 * @param f Flight.
 * @return String com o ID do voo.
 */
const char *get_flight_id(Flight f);

/**
 * @brief Obtém a hora de partida planeada.
 *
 * @param f Flight.
 * @return String com a hora de partida planeada (YYYY-MM-DD HH:MM).
 */
const char *get_flight_dep(Flight f);

#endif