#ifndef LOGICAL_VALIDATION_H
#define LOGICAL_VALIDATION_H

#include <stdbool.h>
#include "entities/flights.h"
#include "entities/reservations.h"
#include "entities/passengers.h"
#include "managers/aircrafts_manager.h"

/**
 * @brief Valida que o destino de um voo é diferente da origem.
 *
 * @param orig Código do aeroporto de origem.
 * @param dest Código do aeroporto de destino.
 * @return true se origem e destino forem diferentes, false caso contrário ou se algum parâmetro for NULL.
 */
bool validate_destination(const char *orig, const char *dest);

/**
 * @brief Valida que horários de chegada e partida são consistentes.
 *
 * Verifica que a chegada e a chegada real não são antes da partida e da partida real.
 * Considera o status do voo (OnTime, Delayed, Cancelled).
 *
 * @param departure Hora de partida planeada (YYYY-MM-DD HH:MM).
 * @param actual_departure Hora de partida real (YYYY-MM-DD HH:MM ou "N/A").
 * @param arrival Hora de chegada planeada (YYYY-MM-DD HH:MM).
 * @param actual_arrival Hora de chegada real (YYYY-MM-DD HH:MM ou "N/A").
 * @param status Estado do voo.
 * @return true se os horários forem consistentes, false caso contrário.
 */
bool validate_arrival(const char *departure,
                      const char *actual_departure,
                      const char *arrival,
                      const char *actual_arrival,
                      flight_status status);

/**
 * @brief Valida que a aeronave de um voo existe no AircraftsManager.
 *
 * @param aircraft_id ID da aeronave.
 * @param am AircraftsManager.
 * @return true se a aeronave existir, false caso contrário.
 */
bool validate_aircraft(const char *aircraft_id, AircraftsManager am);

/**
 * @brief Valida que o estado de um voo é consistente com os horários reais.
 *
 * Para voos cancelados, os horários reais devem ser "N/A".
 *
 * @param status Estado do voo.
 * @param actual_departure Hora de partida real.
 * @param actual_arrival Hora de chegada real.
 * @return true se o estado for consistente, false caso contrário.
 */
bool validate_status(flight_status status,
                     const char *actual_departure,
                     const char *actual_arrival);

#endif
