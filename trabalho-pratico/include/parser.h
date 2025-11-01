#ifndef PARSER_H
#define PARSER_H

#include "dataset.h"
#include <glib.h>


// Each function builds internally the whole path
void parse_airports(Dataset d, const char *data_path);
void parse_flights(Dataset d, const char *data_path);
void parse_aircrafts(Dataset d, const char *data_path);
void parse_passengers(Dataset d, const char *data_path);
void parse_reservations(Dataset d, const char *data_path);

void parse_all(Dataset d, const char *data_path);

#endif