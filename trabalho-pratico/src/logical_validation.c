/**
 * @file logical_validation.c
 * @brief Funções de validação lógica de dados.
 *
 * Este módulo contém funções que validam coerência e regras de negócio
 * entre voos, aeronaves, passageiros e reservas.
 *
 * Validações incluem:
 * - Destino diferente da origem
 * - Consistência de horários de voos
 * - Existência de aeronaves
 * - Coerência entre estado do voo e horários
 * - Existência de passageiros referenciados em reservas
 * - Validação de conexão entre voos de uma mesma reserva
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <glib.h>
#include "logical_validation.h"
#include "flights.h"
#include "aircrafts.h"
#include "reservations.h"
#include "passengers.h"
#include "aircrafts_manager.h"

/**
 * @brief Valida se o destino de um voo é diferente da origem.
 *
 * @param orig Código da origem.
 * @param dest Código do destino.
 * @return true se origem e destino forem diferentes, false caso contrário.
 */
bool validate_destination(const char *orig, const char *dest) {
    if (!orig || !dest) return false;
    return strcmp(orig, dest) != 0;
}

/**
 * @brief Função auxiliar para comparar duas datas no formato "YYYY-MM-DD HH:MM".
 *
 * @param dt1 Primeira data.
 * @param dt2 Segunda data.
 * @return Valor negativo se dt1 < dt2, zero se iguais (ou inválidas), positivo se dt1 > dt2.
 */
static int compare_datetimes(const char *dt1, const char *dt2) {
    if (!dt1 || !dt2) return 0;
    int y1, m1, d1, h1, min1;
    int y2, m2, d2, h2, min2;

    if (sscanf(dt1, "%d-%d-%d %d:%d", &y1, &m1, &d1, &h1, &min1) != 5) return 0;
    if (sscanf(dt2, "%d-%d-%d %d:%d", &y2, &m2, &d2, &h2, &min2) != 5) return 0;

    if (y1 != y2) return y1 - y2;
    if (m1 != m2) return m1 - m2;
    if (d1 != d2) return d1 - d2;
    if (h1 != h2) return h1 - h2;
    return min1 - min2;
}

/**
 * @brief Valida coerência entre horários planejados e reais de um voo.
 *
 * Regras principais:
 *  - arrival >= departure
 *  - actual_arrival >= actual_departure
 *  - Se status = Delayed ou OnTime:
 *       actual_departure >= departure
 *       actual_arrival >= arrival
 *  - Se status = Cancelled → sempre válido
 *
 * @param departure Partida planejada.
 * @param actual_departure Partida real.
 * @param arrival Chegada planejada.
 * @param actual_arrival Chegada real.
 * @param status Estado do voo.
 * @return true se tempos forem coerentes, false caso contrário.
 */
bool validate_arrival(const char *departure,
                      const char *actual_departure,
                      const char *arrival,
                      const char *actual_arrival,
                      flight_status status) {
    if (status == Cancelled) return true;
    if (!departure || !actual_departure || !arrival || !actual_arrival)
        return false;

    if (compare_datetimes(arrival, departure) < 0)
        return false;

    if (compare_datetimes(actual_arrival, actual_departure) < 0)
        return false;

    if (status == Delayed || status == OnTime) {
        if (compare_datetimes(actual_departure, departure) < 0) return false;
        if (compare_datetimes(actual_arrival, arrival) < 0) return false;
    }

    return true;
}

/**
 * @brief Verifica se a aeronave associada a um voo existe no sistema.
 *
 * @param aircraft_id ID da aeronave.
 * @param am Gestor de aeronaves.
 * @return true se a aeronave existir, false caso contrário.
 */
bool validate_aircraft(const char *aircraft_id, AircraftsManager am) {
    if (!am || !aircraft_id || strlen(aircraft_id) == 0) return false;
    return aircrafts_manager_exists(am, aircraft_id);
}

/**
 * @brief Valida coerência entre o estado do voo e os horários reais.
 *
 * - Se status = Cancelled → actual_departure e actual_arrival devem ser "N/A".
 *
 * @param status Estado do voo.
 * @param actual_departure Partida real.
 * @param actual_arrival Chegada real.
 * @return true se válido, false caso contrário.
 */
bool validate_status(flight_status status,
                     const char *actual_departure,
                     const char *actual_arrival) {
    if (!actual_departure || !actual_arrival) return false;

    if (status == Cancelled) {
        return strcmp(actual_departure, "N/A") == 0 &&
               strcmp(actual_arrival, "N/A") == 0;
    }
    return true;
}

/**
 * @brief Valida se o documento associado à reserva corresponde a um passageiro registado.
 *
 * @param r Reserva.
 * @param passengers Lista de passageiros.
 * @param N Número total de passageiros.
 * @return true se o documento existir, false caso contrário.
 */
bool logical_validate_document_number(Reservation r, Passenger *passengers, int N) {
    if (!r || !passengers) return false;
    const char *doc = get_reservation_document_number(r);
    if (!doc) return false;

    for (int i = 0; i < N; i++) {
        const char *pdoc = get_passenger_document_id(passengers[i]);
        if(pdoc && strcmp(doc,pdoc) == 0)
            return true;
    }
    return false;
}

/**
 * @brief Valida ligação entre dois voos de uma reserva de conexão.
 *
 * Regras:
 *  - Só valida reservas com exatamente dois voos.
 *  - O destino do primeiro voo deve corresponder à origem do segundo.
 *
 * @param r Reserva.
 * @param flights Lista de voos existentes.
 * @param num_flights Número de voos existentes.
 * @param num_ids Número de ids na reserva (deve ser 2 para validar).
 * @return true se conexão válida, false caso contrário.
 */
bool validate_reservation_connection(Reservation r, Flight *flights, int num_flights, int num_ids) {
    if (!r|| !flights) return false;
    if (num_ids != 2) return true;

    const Flight *first = NULL, *second = NULL;
    const char *fid1 = get_reservation_flight_id(r, 0);
    const char *fid2 = get_reservation_flight_id(r, 1);

    for (int i = 0; i < num_flights; i++) {
        if (strcmp(get_flight_id(flights[i]), fid1) == 0)
            first = &flights[i];
        else if (strcmp(get_flight_id(flights[i]), fid2) == 0)
            second = &flights[i];
    }

    if (!first || !second) return false;

    const char *fstdest = get_flight_dest(*first);
    const char *sndorig = get_flight_orig(*second);
    if (!fstdest || !sndorig) return false;
    return strcmp(fstdest, sndorig) == 0;
}






