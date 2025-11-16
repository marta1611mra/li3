/**
 * @file passengers.c
 * @brief Implementação das funções para criação e manipulação de passageiros (Passenger).
 *
 * Cada passageiro possui número de documento, nome, data de nascimento, nacionalidade,
 * gênero, email, telefone, endereço e foto.
 */

#include "passengers.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <glib.h>

/**
 * @struct passenger
 * @brief Estrutura que representa um passageiro.
 */
struct passenger {
    char document_id[16]; /**< Número de documento do passageiro. */
    char first_name[30];  /**< Primeiro nome do passageiro. */
    char last_name[30];   /**< Último nome do passageiro. */
    char dob[11];         /**< Data de nascimento (YYYY-MM-DD). */
    char nationality[20]; /**< Nacionalidade do passageiro. */
    char gender[10];      /**< Gênero do passageiro. */
    char email[50];       /**< Email do passageiro. */
    char phone[15];       /**< Telefone do passageiro. */
    char address[50];     /**< Endereço do passageiro. */
    char photo[4096];     /**< Caminho ou base64 da foto do passageiro. */
};

/**
 * @brief Cria um novo passageiro com os dados fornecidos.
 *
 * @param document_id Número de documento.
 * @param first_name Primeiro nome.
 * @param last_name Último nome.
 * @param dob Data de nascimento (YYYY-MM-DD).
 * @param nationality Nacionalidade.
 * @param gender Gênero.
 * @param email Email do passageiro.
 * @param phone Telefone.
 * @param address Endereço.
 * @param photo Foto (caminho ou base64).
 * @return Ponteiro para o passageiro criado ou NULL se falhar a alocação.
 */
Passenger create_passenger(const char *document_id, const char *first_name, const char *last_name, const char *dob, const char *nationality, const char *gender, const char *email, const char *phone, const char *address, const char *photo) {
    Passenger p = g_new0(struct passenger, 1);
    if (!p) return NULL;

    if (document_id)  {
        strncpy(p->document_id, document_id, sizeof(p->document_id) - 1);
        p->document_id[sizeof(p->document_id) - 1] = '\0';
    }
    if (first_name)   strncpy(p->first_name,  first_name,  sizeof(p->first_name) - 1);
    if (last_name)    strncpy(p->last_name,   last_name,   sizeof(p->last_name) - 1);
    if (dob)          strncpy(p->dob,         dob,         sizeof(p->dob) - 1);
    if (nationality)  strncpy(p->nationality, nationality, sizeof(p->nationality) - 1);
    if (gender)       strncpy(p->gender,      gender,      sizeof(p->gender) - 1);
    if (email)        strncpy(p->email,       email,       sizeof(p->email) - 1);
    if (phone)        strncpy(p->phone,       phone,       sizeof(p->phone) - 1);
    if (address)      strncpy(p->address,     address,     sizeof(p->address) - 1);
    if (photo)        strncpy(p->photo,       photo,       sizeof(p->photo) - 1);

    return p;
}

/**
 * @brief Liberta a memória associada a um passageiro.
 *
 * @param p Ponteiro para o passageiro a destruir.
 */
void destroy_passenger(Passenger p) {
    g_free(p);
}

/**
 * @brief Obtém o número de documento do passageiro.
 *
 * @param p Ponteiro para o passageiro.
 * @return Número de documento.
 */
const char *get_passenger_document_id(Passenger p) { 
    return p->document_id; 
}
