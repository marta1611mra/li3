#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <glib.h>
#include "dataset.h"
#include "flights.h"
#include "airports.h"
#include "query3.h"



// Query 3: Find the airport with the most departures between two dates

//Main function
void q3(const char *start_date, const char *end_date, Flights *flights, Airports *airports, FILE *output)
{
    GHashTable *departures = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);
    // Creates a hash table to store departure counts for each airport
    for (int i = 0; i < flights->count; i++){
    // Iterates through all flights to count departures for each airport
        Flight *f = &flights->data[i];

        if (strcmp(f->status, "Cancelled") == 0) 
        // Skips cancelled flights
            continue;

        if (!date_in_range(f->actual_departure, start_date, end_date)) 
        // Skip flights outside the date range
            continue;

        // Get the origin airport and update its count
        const char *origin = f->origin;
        int *count = g_hash_table_lookup(departures, origin);
        if (count)
        {// if the airport already exists in hash table then increment its count
            (*count)++;
        }
        else
        {// if its the first occurrence of this airport, add it to hash table
            int *c = malloc(sizeof(int));
            *c = 1;
            g_hash_table_insert(departures, g_strdup(origin), c);
        }
    }

    // Find the airport with the highest count
    // In case of a tie, choose the one with lexicographically smaller code
    GHashTableIter iter;
    gpointer key, value;
    char *best_code = NULL;
    int max_count = 0;

    g_hash_table_iter_init(&iter, departures);
    while (g_hash_table_iter_next(&iter, &key, &value)){
        char *code = (char *)key;
        int count = *(int *)value;
        // Update best_code if:
        // No airport found yet (NULL)
        // Current airport has more departures
        // Same number of departures but current code is smaller
        if (!best_code || count > max_count ||(count == max_count && strcmp(code, best_code) < 0)){
            best_code = code;
            max_count = count;
        }
    }

    // Output results
    if (best_code){ // output the details of the airport
        Airport *a = get_airport_by_code(airports, best_code);
        fprintf(output, "%s,%s,%s,%s,%d\n",
                a->code, a->name, a->city, a->country, max_count);
            }
    else fprintf(output, "\n"); // No airports with departures found, output empty line
    
    g_hash_table_destroy(departures); // Clean up allocated memory
}



// Other functions

// checks if the actual departure of a flight is between two other dates
bool date_in_range(const char* date, const char* start_date, const char* end_date) {
    return (strcmp(date, start_date) >= 0 && strcmp(date, end_date) <= 0);
}

// gets the airport by it's code IATA
Airport* get_airport_by_code(Airports* airports, const char* code) {
    if (!airports || !code) return NULL;
    for (int i = 0; i < airports->count; i++) {
        if (strcmp(airports->data[i].code, code) == 0) {
            return &airports->data[i];
        }
    }
    return NULL; // not found
}

