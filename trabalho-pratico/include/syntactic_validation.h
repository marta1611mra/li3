#ifndef CSV_VALIDATION_H
#define CSV_VALIDATION_H

#include <stdbool.h>

/**
 * @brief Valida se uma string representa uma data válida (formato YYYY-MM-DD).
 *
 * @param date String contendo a data.
 * @return true se a data for válida, false caso contrário.
 */
bool validate_date(const char *date);

/**
 * @brief Valida se uma string representa uma data e hora válida (formato YYYY-MM-DD HH:MM).
 *
 * @param datetime String contendo a data e hora.
 * @return true se o formato for válido, false caso contrário.
 */
bool validate_datetime(const char *datetime);

/**
 * @brief Valida se uma string representa um ano válido.
 *
 * @param year String contendo o ano.
 * @return true se o ano for válido, false caso contrário.
 */
bool validate_year(const char *year);

/**
 * @brief Valida se uma string é um email válido.
 *
 * @param email String contendo o email.
 * @return true se o email for válido, false caso contrário.
 */
bool validate_email(const char *email);

/**
 * @brief Valida se uma string é um código IATA válido de aeroporto.
 *
 * @param code Código do aeroporto.
 * @return true se o código for válido, false caso contrário.
 */
bool validate_airport_code(const char *code);

/**
 * @brief Valida se uma string representa um género válido.
 *
 * @param gender String contendo o género.
 * @return true se for "M" ou "F" (ou outro definido), false caso contrário.
 */
bool validate_gender(const char *gender);

/**
 * @brief Valida se uma string representa uma latitude válida.
 *
 * @param lat String contendo a latitude.
 * @return true se for válida, false caso contrário.
 */
bool validate_latitude(const char *lat);

/**
 * @brief Valida se uma string representa uma longitude válida.
 *
 * @param lon String contendo a longitude.
 * @return true se for válida, false caso contrário.
 */
bool validate_longitude(const char *lon);

/**
 * @brief Valida se uma latitude e longitude são válidas.
 *
 * @param latitude String contendo a latitude.
 * @param longitude String contendo a longitude.
 * @return true se ambos forem válidos, false caso contrário.
 */
bool validate_latitude_longitude(const char *latitude, const char *longitude);

/**
 * @brief Valida se uma string representa um ID de voo válido.
 *
 * @param id ID do voo.
 * @return true se válido, false caso contrário.
 */
bool validate_flight_id(const char *id);

/**
 * @brief Valida se uma string representa um ID de reserva válido.
 *
 * @param id ID da reserva.
 * @return true se válido, false caso contrário.
 */
bool validate_reservation_id(const char *id);

/**
 * @brief Valida se uma string representa um número de documento válido.
 *
 * @param doc_num Número de documento.
 * @return true se válido, false caso contrário.
 */
bool validate_document_number(const char *doc_num);

/**
 * @brief Valida se uma string representa um tipo de aeroporto válido.
 *
 * @param type Tipo do aeroporto.
 * @return true se válido, false caso contrário.
 */
bool validate_airport_type(const char *type);

/**
 * @brief Valida listas em CSV (ex.: listas de IDs ou códigos separadas por vírgulas).
 *
 * @param list String contendo a lista.
 * @return true se a lista for válida, false caso contrário.
 */
bool validate_csv_lists(const char *list);

#endif
