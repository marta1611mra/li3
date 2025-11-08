#ifndef AIRCRAFTS_H
#define AIRCRAFTS_H

typedef struct aircraft *Aircraft;

Aircraft create_aircraft(const char *id, const char *manufacturer, const char *model, int year, int capacity, int range);

void destroy_aircraft(Aircraft a);

const char *get_aircraft_id(Aircraft a);
const char *get_aircraft_manufacturer(Aircraft a);
const char *get_aircraft_model(Aircraft a);
int get_aircraft_year(Aircraft a);
int get_aircraft_capacity(Aircraft a);
int get_aircraft_range(Aircraft a);


#endif
