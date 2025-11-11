#ifndef AIRPORTS_H
#define AIRPORTS_H

typedef struct airport *Airport;

Airport create_airport(const char *code, const char *name, const char *city, const char *country, double latitude, double longitude, const char *icao, const char *type);

void destroy_airport(Airport a);

const char *get_airport_code(Airport a);

const char *get_airport_name(Airport a);
const char *get_airport_city(Airport a);
const char *get_airport_country(Airport a);
const char *get_airport_type(Airport a);

#endif