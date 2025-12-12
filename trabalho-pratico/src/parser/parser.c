#include "dataset.h"
#include "parser/parser.h"
#include "parser/parser_aircrafts.h"
#include "parser/parser_airports.h"
#include "parser/parser_flights.h"
#include "parser/parser_passangers.h"
#include "parser/parser_reservations.h"
#include "syntactic_validation.h"
#include "logical_validation.h"
#include "flights.h"
#include "reservations.h"
#include "passengers.h"
#include "aircrafts.h"
#include "airports.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <glib.h>


// Verifica se uma string contém apenas dígitos.
bool is_all_digits(const char *s) {
    if (!s || !*s) return false;
    for (const char *p = s; *p; p++) {
        if (!isdigit((unsigned char)*p)) return false;
    }
    return true;
}

// Remove aspas iniciais e finais de uma string.
void remove_quotes(char *str) {
    if(!str) return;
    size_t len = strlen(str);
    if (len == 0) return;
    if (str[0] == '"') memmove(str, str + 1, len);
    len = strlen(str);
    if (len > 0 && str[len - 1] == '"') str[len - 1] = '\0';
}

// Remove espaços em branco do início e fim de uma string.

void remove_spc(char *str) {
    if (!str) return;
    int len = strlen(str);
    
    while (*str && (isspace((unsigned char)*str))) {
        memmove(str, str + 1, strlen(str+1)+1);
    }
    
    while (len > 0 && (isspace((unsigned char)str[len - 1]) ||
                       str[len - 1] == '\r' || str[len - 1] == '\n')) {
        str[--len] = '\0';
    }
}


// Cria a pasta "resultados" caso não exista.
void exist_result(void) {
    system ("mkdir -p resultados");
}

// Copia a linha de cabeçalho do ficheiro CSV para o ficheiro de erros.
void wcsv_header(FILE *csv, FILE *ferror) {
    char header[1000];
    if(fgets(header, sizeof(header), csv)) {
        fprintf(ferror, "%s", header);
    }
}

// Lê e processa todos os ficheiros de dados (airports, flights, aircrafts, passengers, reservations).
void parse_all(Dataset d, const char *data_path){
    exist_result();
    parse_airports(d, data_path);
    parse_aircrafts(d, data_path);
    parse_flights(d, data_path);
    parse_passengers(d, data_path);
    parse_reservations(d, data_path);
}

