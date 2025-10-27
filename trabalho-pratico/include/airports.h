#ifndef AIRPORTS_H
#define AIRPORTS_H

typedef struct airport *Airport;

Airport create_airport(const char *code, const char *name, const char *city, const char *country, double latitude, double longitude, const char *icao, const char *type);

void destroy_airport(Airport a);

#endif