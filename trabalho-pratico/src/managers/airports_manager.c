#include <string.h>
#include "airports_manager.h"
#include "airports.h"
#include <glib.h>
#include <stdlib.h>


// Estrutura que representa um gestor de aeroportos.
struct airports_manager {
    GHashTable *airports; // Tabela hash com chave = código do aeroporto e valor = ponteiro para Airport. 
    GHashTable *arrivals;
    GHashTable *departures;
};


// Cria um gestor de aeroportos.
AirportsManager airports_manager_create() {
    AirportsManager m = malloc(sizeof(*m));
    if (!m) return NULL;

    m->airports = g_hash_table_new_full(g_str_hash, g_str_equal, free, (GDestroyNotify)destroy_airport);
    m->arrivals = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);
    m->departures = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);
    return m;
}


// Destroi um gestor de aeroportos, libertando todos os recursos associados.
void airports_manager_destroy(AirportsManager m) {
    if (!m) return;
    g_hash_table_destroy(m->airports);
    g_hash_table_destroy(m->arrivals);
    g_hash_table_destroy(m->departures);
    free(m);
}


// Adiciona um aeroporto ao gestor.
void airports_manager_add(AirportsManager m, Airport a) {
    if (!m || !a) return;
    
    const char *code = get_airport_code(a);
    g_hash_table_insert(m->airports, strdup(code), a);

    int *arr = malloc(sizeof(int));
    int *dep = malloc(sizeof(int));
    *arr = 0;
    *dep = 0;

    g_hash_table_insert(m->arrivals, strdup(code), arr);
    g_hash_table_insert(m->departures, strdup(code), dep);
}


// Obtém um aeroporto pelo seu código.
Airport airports_manager_get(AirportsManager m, const char *code) {
    return g_hash_table_lookup(m->airports, code);
}

// Obtém o número de aeroportos no gestor.
int airports_manager_count(AirportsManager m) {
    return g_hash_table_size(m->airports);
}

void airports_manager_arrival(AirportsManager m, const char *code, int passengers) {
    if (!m || !code) return;
    int *val = g_hash_table_lookup(m->arrivals, code);
    if (val) *val += passengers;
}

void airports_manager_departure(AirportsManager m, const char *code, int passengers) {
    if (!m || !code) return;
    int *val = g_hash_table_lookup(m->departures, code);
    if (val) *val += passengers;
}

int airports_manager_get_arrivals(AirportsManager m, const char *code) {
    if (!m || !code) return 0;
    int *val = g_hash_table_lookup(m->arrivals, code);
    return val ? *val : 0;
}

int airports_manager_get_departures(AirportsManager m, const char *code) {
    if (!m || !code) return 0;
    int *val = g_hash_table_lookup(m->departures, code);
    return val ? *val : 0;
}