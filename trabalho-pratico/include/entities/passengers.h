#ifndef PASSENGERS_H
#define PASSENGERS_H

/**
 * @brief Opaque pointer para a estrutura Passenger.
 *
 * Representa um passageiro no sistema.
 */
typedef struct passenger *Passenger;

/**
 * @brief Cria um novo passageiro.
 *
 * Inicializa um objeto Passenger com todos os campos fornecidos.
 *
 * @param document_id Documento de identificação único do passageiro (ex.: passaporte ou BI).
 * @param first_name  Primeiro nome.
 * @param last_name   Apelido.
 * @param dob         Data de nascimento (formato YYYY-MM-DD).
 * @param nationality Nacionalidade do passageiro.
 * @param gender      Género do passageiro (ex.: "M", "F").
 * @param email       Email do passageiro.
 * @param phone       Número de telefone.
 * @param address     Morada do passageiro.
 * @param photo       Caminho para a fotografia ou URL (opcional, pode ser NULL).
 * @return Ponteiro para o novo Passenger criado. NULL se falhar a alocação.
 */
Passenger create_passenger(const char *document_id, const char *first_name, const char *last_name, const char *dob, const char *nationality, const char *gender, const char *email, const char *phone, const char *address, const char *photo);

/**
 * @brief Liberta a memória associada a um passageiro.
 *
 * @param p Passenger a destruir. Ignorado se for NULL.
 */
void destroy_passenger(Passenger p);

/**
 * @brief Obtém o número de documento do passageiro.
 *
 * @param p Passenger.
 * @return String com o documento de identificação.
 */
const char *get_passenger_document_id(Passenger p);

#endif