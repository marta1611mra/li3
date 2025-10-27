#ifndef PARSER_H
#define PARSER_H

#include "aircrafts.h"
#include "airports.h"
#include "flights.h"
#include "passengers.h"
#include "reservations.h"

// forward declaration, it allows header to manipulate pointers to Datasets without needing to know their internal structure
typedef struct dataset *Dataset;


Dataset create_dataset(); // allocates memory and initializes the struct Dataset
void destroy_dataset(Dataset d); // frees all the memory associated to dataset

// Each function builds internally the whole path
void parse_airports(Dataset d, const char *data_path);
void parse_flights(Dataset d, const char *data_path);
void parse_aircrafts(Dataset d, const char *data_path);
void parse_passengers(Dataset d, const char *data_path);
void parse_reservations(Dataset d, const char *data_path);

#endif