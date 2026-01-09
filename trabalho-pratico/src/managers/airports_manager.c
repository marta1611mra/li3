#include <string.h>
#include "airports_manager.h"
#include "airports.h"
#include <glib.h>
#include <stdlib.h>


// Estrutura que representa um gestor de aeroportos.
struct airports_manager {
    GHashTable *airports; // Tabela hash com chave = código do aeroporto e valor = ponteiro para Airport. 
};


// Cria um gestor de aeroportos.
AirportsManager airports_manager_create() {
    AirportsManager m = malloc(sizeof(*m));
    if (!m) return NULL;

    m->airports = g_hash_table_new_full(g_str_hash, g_str_equal, free, (GDestroyNotify)destroy_airport);
    return m;
}


// Destroi um gestor de aeroportos, libertando todos os recursos associados.
void airports_manager_destroy(AirportsManager m) {
    if (!m) return;
    g_hash_table_destroy(m->airports);
    free(m);
}


// Adiciona um aeroporto ao gestor.
void airports_manager_add(AirportsManager m, Airport a) {
    if (!m || !a) return;
    
    const char *code = get_airport_code(a);
    g_hash_table_insert(m->airports, strdup(code), a);
}


// Obtém um aeroporto pelo seu código.
Airport airports_manager_get(AirportsManager m, const char *code) {
    return g_hash_table_lookup(m->airports, code);
}

void airports_manager_arrival(AirportsManager m, const char *code, int passengers) {
Airport a = airports_manager_get(m, code);
    if (a) airport_add_arrival_passengers(a, passengers);
}

void airports_manager_departure(AirportsManager m, const char *code, int passengers) {
Airport a = airports_manager_get(m, code);
    if (a) airport_add_departure_passengers(a, passengers);
}



// Obtém o total de chegadas de um aeroporto
int airports_manager_get_arrivals(AirportsManager m, const char *code) {
    // 1. Procura o aeroporto na tabela principal
    Airport a = g_hash_table_lookup(m->airports, code);
    
    // 2. Se o aeroporto não existir, retorna 0
    if (!a) return 0;
    
    // 3. Pede o valor à entidade (usando a função nova que criaste no airports.c)
    return get_airport_arrivals_count(a);
}

// Obtém o total de partidas de um aeroporto
int airports_manager_get_departures(AirportsManager m, const char *code) {
    // 1. Procura o aeroporto na tabela principal
    Airport a = g_hash_table_lookup(m->airports, code);
    
    if (!a) return 0;
    
    // 2. Pede o valor à entidade
    return get_airport_departures_count(a);
}