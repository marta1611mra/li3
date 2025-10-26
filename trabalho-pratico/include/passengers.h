#ifndef PASSENGERS_H
#define PASSENGERS_H

typedef struct passenger *Passenger;

Passenger create_passenger(const char *document_id, const char *first_name, const char *last_name, const char *dob, const char *nationality, const char *gender, const char *email, const char *phone, const char *address, const char *photo);

void destroy_passenger(Passenger p){
    if (p) free(p);
}
#endif

